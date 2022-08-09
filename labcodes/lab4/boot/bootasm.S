#include <asm.h>

# Start the CPU: switch to 32-bit protected mode, jump into C.
# The BIOS loads this code from the first sector of the hard disk into
# memory at physical address 0x7c00 and starts executing in real mode
# with %cs=0 %ip=7c00.

.set PROT_MODE_CSEG,        0x8                     # kernel code segment selector
.set PROT_MODE_DSEG,        0x10                    # kernel data segment selector
.set CR0_PE_ON,             0x1                     # protected mode enable flag
.set SMAP,                  0x534d4150

# start address should be 0:7c00, in real mode, the beginning address of the running bootloader
.globl start
start:
.code16                                             # Assemble for 16-bit mode
    cli                                             # Disable interrupts
    cld                                             # String operations increment

    # Set up the important data segment registers (DS, ES, SS).
    xorw %ax, %ax                                   # Segment number zero
    movw %ax, %ds                                   # -> Data Segment
    movw %ax, %es                                   # -> Extra Segment
    movw %ax, %ss                                   # -> Stack Segment

    # Enable A20:
    #  For backwards compatibility with the earliest PCs, physical
    #  address line 20 is tied low, so that addresses higher than
    #  1MB wrap around to zero by default. This code undoes this.

seta20.1:
    inb $0x64, %al                                  # Wait for not busy(8042 input buffer empty).
    testb $0x2, %al                                 # 如果 %al 第低2位为1，则ZF = 0, 则跳转
    jnz seta20.1                                    # 如果 %al 第低2位为0，则ZF = 1, 则不跳转

    movb $0xd1, %al                                 # 0xd1 -> port 0x64
    outb %al, $0x64                                 # 0xd1 means: write data to 8042's P2 port

seta20.2:
    inb $0x64, %al                                  # Wait for not busy(8042 input buffer empty).
    testb $0x2, %al
    jnz seta20.2

    movb $0xdf, %al                                 # 0xdf -> port 0x60
    outb %al, $0x60                                 # 0xdf = 11011111, means set P2's A20 bit(the 1 bit) to 1

# 探针探测内存
probe_memory:
# 对0x8000处的32位单元清零,即给位于0x8000处的
# struct e820map的成员变量nr_map清零
    movl $0, 0x8000         # 0x8000处清零
    xorl %ebx, %ebx         # ebx寄存器通过异或清零
    # SI是源变址寄存器，DI是目的变址寄存器,
    # 在串处理指令中，SI用作隐含的源串地址，默认在DS中；DI用做隐含的目的串地址，默认在ES中；此时不能混用。
    # %di寄存器放$0x8004，即0x8000 + 0x4
    # 表示设置调用INT 15h BIOS中断后，BIOS返回的映射地址描述符的起始地址
    movw $0x8004, %di       
    
start_probe:
    movl $0xE820, %eax      # eax 为0xE820,INT 15的中断调用参数
    movl $20, %ecx          # 设置地址范围描述符的大小为20字节，其大小等于struct e820map的成员变量map的大小
    movl $SMAP, %edx        # SMAP为0x534d4150,设置edx为534D4150h (即4个ASCII字符“SMAP”)，这是一个约定
    int $0x15               # int 0x15中断，要求BIOS返回一个用地址范围描述符表示的内存段信息
    jnc cont                # 如果eflags的CF位为0，则表示还有内存段需要探测,JNC 意思是CF=0，则跳转，表示的是AX大于或等于0的时候转移到EXIT处，所以JNC也可以看成是加法没有进位，减法没有借位的时候转移。
    movw $12345, 0x8000     # 探测有问题，结束探测
    jmp finish_probe        # 跳转finish_probe
cont:
    addw $20, %di           # di + 20,设置下一个BIOS返回的映射地址描述符的起始地址
    incl 0x8000             # 自增指令inc(类似c里的++i),递增struct e820map的成员变量nr_map
    cmpl $0, %ebx           # 如果INT 0x15返回的ebx为零，表示探测结束，否则继续探测,cmpl指令将两个操作数相减，但计算结果并不保存，只是根据计算结果改变eflags寄存器中的标志位。如果两个操作数相等，则计算结果为0，eflags中的ZF位置1。
    jnz start_probe         # JNE/JNZ 不等于时转移.
finish_probe:
    # 全局描述符表：存放8字节的段描述符，段描述符包含段的属性。
    # 段选择符：总共16位，高13位用作全局描述符表中的索引位，GDT的第一项总是设为0，
    # 因此段选择符的逻辑地址会被认为是无效的，从而引起一个处理器异常。GDT表项
    # 最大数目是8191个，即2^13 - 1.
    # Switch from real to protected mode, using a bootstrap GDT
    # and segment translation that makes virtual addresses
    # identical to physical addresses, so that the
    # effective memory map does not change during the switch.
    lgdt gdtdesc
    movl %cr0, %eax
    orl $CR0_PE_ON, %eax
    movl %eax, %cr0

    # Jump to next instruction, but in 32-bit code segment.
    # Switches processor into 32-bit mode.
    # ljmp是长转移指令（64kb范围内转移指令）：把指令码中的目标地址addr16装入程序计数器PC，
    # 使机器执行下一条指令时无条件转移到addr16处执行程序，不影响任何标志。
    # 由于addr16是一个16位二进制地址（地址范围为0000H—FFFFH），
    # 因此长转移指令一条可以在64KB范围内转移的指令。为了使程序设计方便易编，
    # addr16常采用标号地址（如：LOOP、LOOP1、MAIN、START、DONE、NEXT1……）表示，
    # 只有在上机执行前才被汇编（或代真）为16位二进制地址。
    ljmp $PROT_MODE_CSEG, $protcseg

.code32                                             # Assemble for 32-bit mode
protcseg:
    # Set up the protected-mode data segment registers
    movw $PROT_MODE_DSEG, %ax                       # Our data segment selector
    movw %ax, %ds                                   # -> DS: Data Segment
    movw %ax, %es                                   # -> ES: Extra Segment
    movw %ax, %fs                                   # -> FS
    movw %ax, %gs                                   # -> GS
    movw %ax, %ss                                   # -> SS: Stack Segment

    # Set up the stack pointer and call into C. The stack region is from 0--start(0x7c00)
    movl $0x0, %ebp
    movl $start, %esp
    call bootmain

    # If bootmain returns (it shouldn't), loop.
spin:
    jmp spin

.data
# Bootstrap GDT
.p2align 2                                          # force 4 byte alignment
gdt:
    SEG_NULLASM                                     # null seg
    SEG_ASM(STA_X|STA_R, 0x0, 0xffffffff)           # code seg for bootloader and kernel
    SEG_ASM(STA_W, 0x0, 0xffffffff)                 # data seg for bootloader and kernel

gdtdesc:
    .word 0x17                                      # 2bytes sizeof(gdt) - 1 = 24 - 1 = 23
    .long gdt                                       # 4bytes address gdt

# .word 0x17
# 
# /* Normal segment */
# #define SEG_NULLASM                                             \
#     .word 0, 0;                                                 \
#     .byte 0, 0, 0, 0
# 
# #define SEG_ASM(type,base,lim)                                  \
#     .word (((lim) >> 12) & 0xffff), ((base) & 0xffff);          \
#     .byte (((base) >> 16) & 0xff), (0x90 | (type)),             \
#         (0xC0 | (((lim) >> 28) & 0xf)), (((base) >> 24) & 0xff)
# # NULL SEGMENT
# 00000000 00000000 00000000 00000000
# 00000000 00000000 00000000 00000000
# # CODE SEGMENT
# 11111111 11001111 10011010 11111111
# 11111111 11111111 11111111 11111111
# # DATA SEGMENT
# 11111111 11001111 10010010 11111111
# 11111111 11111111 11111111 11111111


(gdb) x /10x $di
0x1:    0x53f000ff      0xc3f000ff      0x53f000e2      0x53f000ff
0x11:   0x53f000ff      0x53f000ff      0x53f000ff      0xa5f000ff
0x21:   0x87f000fe      0x2cf000e9

01010011111100000000000011111111        0x53f000ff
11000011111100000000000011111111        0xc3f000ff
01010011111100000000000011100010        0x53f000e2
01010011111100000000000011111111        0x53f000ff