#include <defs.h>
#include <stdio.h>
#include <string.h>
#include <console.h>
#include <kdebug.h>
#include <picirq.h>
#include <trap.h>
#include <clock.h>
#include <intr.h>
#include <pmm.h>
#include <vmm.h>
#include <ide.h>
#include <swap.h>
#include <proc.h>
#include <fs.h>
#include <kmonitor.h>

int kern_init(void) __attribute__((noreturn));
void grade_backtrace(void);
static void lab1_switch_test(void);

int
kern_init(void) {
    // TODO 需要搞清楚作用
    // “edata”表示数据段的结束地址“.bss”表示数据段的结束地址和BSS段的起始地址，而“end”表示BSS段的结束地址。
    extern char edata[], end[];
    memset(edata, 0, end - edata);

    cons_init();                // init the console

    const char *message = "(THU.CST) os is loading ...";
    cprintf("%s\n\n", message);

    print_kerninfo();

    // 最后调用到print_stackframe
    // 看不出来作用？可能是要试一下给C执行环境准备的栈是否可用？
    grade_backtrace();

    pmm_init();                 // init physical memory management

    pic_init();                 // init interrupt controller
    idt_init();                 // init interrupt descriptor table

    vmm_init();                 // init virtual memory management
    sched_init();               // init scheduler
    proc_init();                // init process table
    
    ide_init();                 // init ide devices
    swap_init();                // init swap
    fs_init();                  // init fs
    
    clock_init();               // init clock interrupt
    intr_enable();              // enable irq interrupt

    //LAB1: CAHLLENGE 1 If you try to do it, uncomment lab1_switch_test()
    // user/kernel mode switch test
    //lab1_switch_test();
    
    cpu_idle();                 // run idle process
}

void __attribute__((noinline))
grade_backtrace2(int arg0, int arg1, int arg2, int arg3) {
    mon_backtrace(0, NULL, NULL);
}

void __attribute__((noinline))
grade_backtrace1(int arg0, int arg1) {
    grade_backtrace2(arg0, (int)&arg0, arg1, (int)&arg1);
}

void __attribute__((noinline))
grade_backtrace0(int arg0, int arg1, int arg2) {
    grade_backtrace1(arg0, arg2);
}

void
grade_backtrace(void) {
    grade_backtrace0(0, (int)kern_init, 0xffff0000);
}

static void
lab1_print_cur_status(void) {
    static int round = 0;
    uint16_t reg1, reg2, reg3, reg4;
    asm volatile (
            "mov %%cs, %0;"
            "mov %%ds, %1;"
            "mov %%es, %2;"
            "mov %%ss, %3;"
            : "=m"(reg1), "=m"(reg2), "=m"(reg3), "=m"(reg4));
    cprintf("%d: @ring %d\n", round, reg1 & 3);
    cprintf("%d:  cs = %x\n", round, reg1);
    cprintf("%d:  ds = %x\n", round, reg2);
    cprintf("%d:  es = %x\n", round, reg3);
    cprintf("%d:  ss = %x\n", round, reg4);
    round ++;
}

static void
lab1_switch_to_user(void) {

/**
--------------------------------------------------------
	"sub $0x8, %%esp \n" 
	让 SS 和 ESP 这两个寄存器 有机会 POP 出时 更新 SS 和 ESP
	因为 从内核态进入中断 它的特权级没有改变 是不会 push 进 SS 和 ESP的 但是我们又需要通过 POP SS 和 ESP 去修改它们
	进入 T_SWITCH_TOU(120) 中断
	将原来的栈顶指针还给esp栈底指针
--------------------------------------------------------
*/

    //LAB1 CHALLENGE 1 : TODO
    __asm__ __volatile__ (
        "sub $0x8, %%esp \n"
        "INT %0 \n"
        "movl %%ebp, %%esp \n"       // 因为这里主动调INT之后，汇编不会帮我们把类似leave的指令补齐，所以需要我们自己加上去
        :
        :"irq"(T_SWITCH_TOU)
    );

/** 
 * ----------------------------第二种方式-----------------------------------
 * 中断发生前压入的SS实际不会被使用，所以代码中仅仅是压入了%eax占位
 * 主动push esp
 * 	__asm__ __volatile__ (
		"pushl %%eax\n\t"
		"pushl %%esp\n\t"
		"int %0\n\t"
		:
		:"i" (T_SWITCH_TOU)
	);
 * ----------------------------第二种方式-----------------------------------
 * /

/**
--------------------------------------------------------
中断处理例程处于ring 0,所以内核态发生的中断不发生堆栈切换，因此SS、ESP不会自动压栈;
但是是否弹出SS、ESP确实由堆栈上的CS中的特权位决定的。
当我们将堆栈中的CS的特权位设置为ring 3时，IRET会误认为中断是从ring 3发生的，
执行时会按照发生特权级切换的情况弹出SS、ESP。

利用这个特性，只需要手动地将内核堆栈布局设置为发生了特权级转换时的布局，
将所有的特权位修改为DPL_USER,保持EIP、ESP不变，IRET执行后就可以切换为应用态。

因为从内核态发生中断不压入SS、ESP，所以在中断前手动压入SS、ESP。
中断处理过程中会修改tf->tf_esp的值，中断发生前压入的SS实际不会被使用，所以代码中仅仅是压入了%eax占位。

为了在切换为应用态后，保存原有堆栈结构不变，确保程序正确运行，栈顶的位置应该被恢复到中断发生前的位置。
SS、ESP是通过push指令压栈的，压入SS后，ESP的值已经上移了4个字节，所以在trap_dispatch将ESP下移4字节。
为了保证在用户态下也能使用I/O，将IOPL降低到了ring 3。
--------------------------------------------------------
*/
}

static void
lab1_switch_to_kernel(void) {
    //LAB1 CHALLENGE 1 :  TODO
    // 当内核初始完毕后，可从内核态返回到用户态的函数
    __asm__ __volatile__ (
        "INT %0 \n"
        "movl %%ebp, %%esp \n"
        :
        :"irq"(T_SWITCH_TOK)
    );
}

static void
lab1_switch_test(void) {
    lab1_print_cur_status();
    cprintf("+++ switch to  user  mode +++\n");
    lab1_switch_to_user();
    lab1_print_cur_status();
    cprintf("+++ switch to kernel mode +++\n");
    lab1_switch_to_kernel();
    lab1_print_cur_status();
}

