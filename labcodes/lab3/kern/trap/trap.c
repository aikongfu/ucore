#include <defs.h>
#include <mmu.h>
#include <memlayout.h>
#include <clock.h>
#include <trap.h>
#include <x86.h>
#include <stdio.h>
#include <assert.h>
#include <console.h>
#include <vmm.h>
#include <swap.h>
#include <kdebug.h>
#include <string.h>

#define TICK_NUM 100

static void print_ticks() {
    cprintf("%d ticks\n",TICK_NUM);
#ifdef DEBUG_GRADE
    cprintf("End of Test.\n");
    panic("EOT: kernel seems ok.");
#endif
}

/* *
 * Interrupt descriptor table:
 *
 * Must be built at run time because shifted function addresses can't
 * be represented in relocation records.
 * */
static struct gatedesc idt[256] = {{0}};

static struct pseudodesc idt_pd = {
    sizeof(idt) - 1, (uintptr_t)idt
};

/* idt_init - initialize IDT to each of the entry points in kern/trap/vectors.S */
void
idt_init(void) {
     /* LAB1 YOUR CODE : STEP 2 */
     /* (1) Where are the entry addrs of each Interrupt Service Routine (ISR)?
      *     All ISR's entry addrs are stored in __vectors. where is uintptr_t __vectors[] ?
      *     __vectors[] is in kern/trap/vector.S which is produced by tools/vector.c
      *     (try "make" command in lab1, then you will find vector.S in kern/trap DIR)
      *     You can use  "extern uintptr_t __vectors[];" to define this extern variable which will be used later.
      * (2) Now you should setup the entries of ISR in Interrupt Description Table (IDT).
      *     Can you see idt[256] in this file? Yes, it's IDT! you can use SETGATE macro to setup each item of IDT
      * (3) After setup the contents of IDT, you will let CPU know where is the IDT by using 'lidt' instruction.
      *     You don't know the meaning of this instruction? just google it! and check the libs/x86.h to know more.
      *     Notice: the argument of lidt is idt_pd. try to find it!
      */
  // ������ʾ������Ҫ__vectors��extern���ⲿ����������__vectors��ͨ��tools/vector.c���ɵ�vectors.S���涨���
  extern uintptr_t __vectors[];
  // ��2562���ж��������ʼ��
  int i;
  for (i = 0; i < (sizeof(idt) / (sizeof(struct gatedesc))); i++)
  {
    // idt���������ÿһ����Ҳ������ָ���ʾ��
    // 0��ʾ��һ��interrupt gate
    // segment selector����ΪGD_KTEXT������Σ�
    // offset����Ϊ__vectors��Ӧ������
    // DPL����Ϊ0
    SETGATE(idt[i], 0, GD_KTEXT, __vectors[i], DPL_KERNEL);
	  // �ٰѴ��û�̬�л����ں�̬ʹ�õ�Segment Descriptor��һ��
  // ��Ҫע����ǣ�����ʹ�õ�segment����һ���ģ�����GD_KTEXT
  // ����һ�㲻ͬ���������DPL��DPL_USER������user->kernelʱ����Ҫ�ĸöε�Ȩ�޼���
  // ��ΪPrivilege Check��Ҫ���㣺DPL >= max {CPL, RPL} 
  // �����������������������Privilege Checkʧ�ܣ��޷���ȷ����user->kernel������
  SETGATE(idt[T_SWITCH_TOK], 0, GD_KTEXT, __vectors[T_SWITCH_TOK], DPL_USER);

  // ͨ��lidt����
  lidt(&idt_pd);
  }
}

static const char *
trapname(int trapno) {
    static const char * const excnames[] = {
        "Divide error",
        "Debug",
        "Non-Maskable Interrupt",
        "Breakpoint",
        "Overflow",
        "BOUND Range Exceeded",
        "Invalid Opcode",
        "Device Not Available",
        "Double Fault",
        "Coprocessor Segment Overrun",
        "Invalid TSS",
        "Segment Not Present",
        "Stack Fault",
        "General Protection",
        "Page Fault",
        "(unknown trap)",
        "x87 FPU Floating-Point Error",
        "Alignment Check",
        "Machine-Check",
        "SIMD Floating-Point Exception"
    };

    if (trapno < sizeof(excnames)/sizeof(const char * const)) {
        return excnames[trapno];
    }
    if (trapno >= IRQ_OFFSET && trapno < IRQ_OFFSET + 16) {
        return "Hardware Interrupt";
    }
    return "(unknown trap)";
}

/* trap_in_kernel - test if trap happened in kernel */
bool
trap_in_kernel(struct trapframe *tf) {
    return (tf->tf_cs == (uint16_t)KERNEL_CS);
}

static const char *IA32flags[] = {
    "CF", NULL, "PF", NULL, "AF", NULL, "ZF", "SF",
    "TF", "IF", "DF", "OF", NULL, NULL, "NT", NULL,
    "RF", "VM", "AC", "VIF", "VIP", "ID", NULL, NULL,
};

void
print_trapframe(struct trapframe *tf) {
    cprintf("trapframe at %p\n", tf);
    print_regs(&tf->tf_regs);
    cprintf("  ds   0x----%04x\n", tf->tf_ds);
    cprintf("  es   0x----%04x\n", tf->tf_es);
    cprintf("  fs   0x----%04x\n", tf->tf_fs);
    cprintf("  gs   0x----%04x\n", tf->tf_gs);
    cprintf("  trap 0x%08x %s\n", tf->tf_trapno, trapname(tf->tf_trapno));
    cprintf("  err  0x%08x\n", tf->tf_err);
    cprintf("  eip  0x%08x\n", tf->tf_eip);
    cprintf("  cs   0x----%04x\n", tf->tf_cs);
    cprintf("  flag 0x%08x ", tf->tf_eflags);

    int i, j;
    for (i = 0, j = 1; i < sizeof(IA32flags) / sizeof(IA32flags[0]); i ++, j <<= 1) {
        if ((tf->tf_eflags & j) && IA32flags[i] != NULL) {
            cprintf("%s,", IA32flags[i]);
        }
    }
    cprintf("IOPL=%d\n", (tf->tf_eflags & FL_IOPL_MASK) >> 12);

    if (!trap_in_kernel(tf)) {
        cprintf("  esp  0x%08x\n", tf->tf_esp);
        cprintf("  ss   0x----%04x\n", tf->tf_ss);
    }
}

void
print_regs(struct pushregs *regs) {
    cprintf("  edi  0x%08x\n", regs->reg_edi);
    cprintf("  esi  0x%08x\n", regs->reg_esi);
    cprintf("  ebp  0x%08x\n", regs->reg_ebp);
    cprintf("  oesp 0x%08x\n", regs->reg_oesp);
    cprintf("  ebx  0x%08x\n", regs->reg_ebx);
    cprintf("  edx  0x%08x\n", regs->reg_edx);
    cprintf("  ecx  0x%08x\n", regs->reg_ecx);
    cprintf("  eax  0x%08x\n", regs->reg_eax);
}

struct trapframe switchk2u, *switchu2k;

/**
 *
 */
void switch2user(struct trapframe *tf)
{
  // eflags
  // 0x3000 = 00110000 00000000
  // ��nested taskλ��1��Ҳ���ǿ���Ƕ��
  tf->tf_eflags |= 0x3000;

  // USER_CS = 3 << 3 | 3 = 24 | 3 = 27 = 0x1B = 00011011;
  // �����ǰ���еĳ��������û�̬�Ĵ����ִ�У����ں��л������϶������ǣ�
  if (tf->tf_cs != USER_CS)
  {
    switchk2u = *tf;
    switchk2u.tf_cs = USER_CS;
    // �������ݶ�ΪUSER_DS
    switchk2u.tf_ds = switchk2u.tf_es = switchk2u.tf_ss = USER_DS;
    // ��Ϊ�ڴ��ǴӸߵ��ͣ�
    // �����Ǵ��ں�̬�л����û�̬��û��ss,sp��
    // (uint32_t)tf + sizeof(struct trapframe) - 8 �� tf->tf_esp�ĵ�ַ
    // Ҳ����switchk2u.tf_esp��ָ��ɵ�tf_esp��ֵ
    switchk2u.tf_esp = (uint32_t)tf + sizeof(struct trapframe) - 8;

    //  eflags ����IOPL
    switchk2u.tf_eflags | FL_IOPL_MASK;

    // (uint32_t *)tf��һ��ָ�룬ָ��ĵ�ַ-1��
    // *((uint32_t *)tf - 1) ���ָ��ָ��ĵ�ַ����Ϊ�����·���������tss(switchk2u)

    *((uint32_t *)tf - 1) = (uint32_t)&switchk2u;
  }
}

void switch2kernel(struct trapframe *tf)
{
  if (tf->tf_cs != KERNEL_CS)
  {
    // ����CSΪ KERNEL_CS = 0x8 = 1000 =  00001|0|00 -> Index = 1, GDT, RPL = 0 
    tf->tf_cs = KERNEL_CS;
    // KERNEL_DS = 00010|0|00 -> Index = 2, GDT, RPL = 0 
    tf->tf_ds = tf->tf_es = KERNEL_DS;

    // FL_IOPL_MASK = 0x00003000 = 0011000000000000 = 00110000 00000000
    // I/O Privilege Level bitmask
    // tf->tf_eflags = (tf->tf_eflags) & (~FL_IOPL_MASK)
    // = (tf->tf_eflags) & (11111111 11111111 11001111 11111111)
    // Ҳ���ǰ�IOPL����Ϊ0
    // IOPL(bits 12 and 13) [I/O privilege level field]   
    // ָʾ��ǰ���������I/O��Ȩ��(I/O privilege level)��
    // ������������ĵ�ǰ��Ȩ��(CPL)����С�ڻ����I/O��Ȩ�������������I/O��ַ�ռ䡣
    // �����ֻ����CPLΪ0ʱ����ͨ��POPF�Լ�IRETָ���޸ġ�
    tf->tf_eflags &= ~FL_IOPL_MASK;

    // �����ڴ沼���ǴӸߵ��ͣ����������޸�switchu2k��ָ��
    switchu2k = (struct trapframe *)(tf->tf_esp - (sizeof(struct trapframe) - 8));

    /* *
    * memmove - copies the values of @n bytes from the location pointed by @src to
    * the memory area pointed by @dst. @src and @dst are allowed to overlap.
    * @dst        pointer to the destination array where the content is to be copied
    * @src        pointer to the source of data to by copied
    * @n:        number of bytes to copy
    *
    * The memmove() function returns @dst.
    * */
    // �൱���ǰ�tf��������switchu2k
    memmove(switchu2k, tf, sizeof(struct trapframe) - 8);

    // �޸�tf - 1����ָ���µ�trapframe
    *((uint32_t *)tf - 1) = (uint32_t)switchu2k;
  }
}

static inline void
print_pgfault(struct trapframe *tf) {
    /* error_code:
     * bit 0 == 0 means no page found, 1 means protection fault
     * bit 1 == 0 means read, 1 means write
     * bit 2 == 0 means kernel, 1 means user
     * */
    cprintf("page fault at 0x%08x: %c/%c [%s].\n", rcr2(),
            (tf->tf_err & 4) ? 'U' : 'K',
            (tf->tf_err & 2) ? 'W' : 'R',
            (tf->tf_err & 1) ? "protection fault" : "no page found");
}

static int
pgfault_handler(struct trapframe *tf) {
    extern struct mm_struct *check_mm_struct;
    print_pgfault(tf);
    if (check_mm_struct != NULL) {
        return do_pgfault(check_mm_struct, tf->tf_err, rcr2());
    }
    panic("unhandled page fault.\n");
}

static volatile int in_swap_tick_event = 0;
extern struct mm_struct *check_mm_struct;

static void
trap_dispatch(struct trapframe *tf) {
    char c;

    int ret;

    switch (tf->tf_trapno) {
    case T_PGFLT:  //page fault
        if ((ret = pgfault_handler(tf)) != 0) {
            print_trapframe(tf);
            panic("handle pgfault failed. %e\n", ret);
        }
        break;
#if 0
    LAB3 : If some page replacement algorithm(such as CLOCK PRA) need tick to change the priority of pages, 
    then you can add code here. 
#endif
    case IRQ_OFFSET + IRQ_TIMER:
        /* LAB1 YOUR CODE : STEP 3 */
        /* handle the timer interrupt */
        /* (1) After a timer interrupt, you should record this event using a global variable (increase it), such as ticks in kern/driver/clock.c
         * (2) Every TICK_NUM cycle, you can print some info using a funciton, such as print_ticks().
         * (3) Too Simple? Yes, I think so!
         */
        ticks++;
        if (ticks % TICK_NUM == 0) {
            print_ticks();
        }
        break;
    case IRQ_OFFSET + IRQ_COM1:
        c = cons_getc();
        cprintf("serial [%03d] %c\n", c, c);
        break;
    case IRQ_OFFSET + IRQ_KBD:
        c = cons_getc();
        cprintf("kbd [%03d] %c\n", c, c);
		
		/*********************/
		//Hardware Interrupt is different with software trap, so no need use temp stack
		
		//if keyboard input '3' it will go to USER mode
		
		if ( c =='3'){
			switch2user(tf);	
			//the status can show in trapframe, 
			//however register value change at iret in trapentry.s,
			//so lab1_print_cur_status() does not work
			print_trapframe(tf);
			//lab1_print_cur_status();
		}
		
		//if keyboard input '0' it will go to Kernel mode
		if ( c =='0'){

			switch2kernel(tf);
			print_trapframe(tf);
			//lab1_print_cur_status();
		}		
		break;
  // LAB1 CHALLENGE 1 : YOUR CODE you should modify below codes.
  case T_SWITCH_TOU:
    // USER_CS = 3 << 3 | 3 = 24 | 3 = 27 = 0x1B = 00011011
    switch2user(tf);
    break;
  case T_SWITCH_TOK:
    // panic("T_SWITCH_** ??\n");
    switch2kernel(tf);
    break;
  case IRQ_OFFSET + IRQ_IDE1:
  case IRQ_OFFSET + IRQ_IDE2:
    /* do nothing */
    break;
  default:
    // in kernel, it must be a mistake
    if ((tf->tf_cs & 3) == 0)
    {
      print_trapframe(tf);
      panic("unexpected trap in kernel.\n");
    }
  }
}

/* *
 * trap - handles or dispatches an exception/interrupt. if and when trap() returns,
 * the code in kern/trap/trapentry.S restores the old CPU state saved in the
 * trapframe and then uses the iret instruction to return from the exception.
 * */
void
trap(struct trapframe *tf) {
    // dispatch based on what type of trap occurred
    trap_dispatch(tf);
}

