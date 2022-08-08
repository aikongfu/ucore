#include <mmu.h>
#include <memlayout.h>

#define REALLOC(x) (x - KERNBASE)

# kern_entry函数的主要任务是为执行kern_init建立一个良好的C语言运行环境（设置堆栈），
# 而且临时建立了一个段映射关系，为之后建立分页机制的过程做一个准备（细节在3.5小节有进一步阐述）

# 之前直接调用init.c的kern_init，现在先到entry.S
# 参考kernel.ld里面的ENTRY(kern_entry)

# (2 << 3) | 0 = 16 = 0x10 = 00010000 = 00000000 00010|0|00 -> Index = 2, GDT, RPL = 0 
# #define KERNEL_DS    ((GD_KDATA) | DPL_KERNEL)
.text
.globl kern_entry
kern_entry:
    # 地址映射的过渡阶段
    # 在bootasm.S里面已经设置过gdt了，这里重新设置一下，重装映射物理内存
    # 虚地址的 0~4G = 线性地址&物理地址-KERNBASE ~ 4G-KERNBASE 
    # reload temperate gdt (second time) to remap all physical memory
    # virtual_addr 0~4G=linear_addr&physical_addr -KERNBASE~4G-KERNBASE 
    lgdt REALLOC(__gdtdesc)     # REALLOC(x) (x - KERNBASE)
    movl $KERNEL_DS, %eax       # 开始设置数据段
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss

    ljmp $KERNEL_CS, $relocated    # ljmp $section,$offset即跳转到section:offset地址。

relocated:

    # set ebp, esp
    movl $0x0, %ebp     # ebp 设置为0，即栈基址寄存器设置为0 
    # the kernel stack region is from bootstack -- bootstacktop,
    # the kernel stack size is KSTACKSIZE (8KB)defined in memlayout.h
    movl $bootstacktop, %esp    
    # now kernel stack is ready , call the first C function
    call kern_init

# should never get here
spin:
    jmp spin

.data
.align PGSIZE
    .globl bootstack
bootstack:
    .space KSTACKSIZE
    .globl bootstacktop
bootstacktop:

.align 4
__gdt:
    SEG_NULL
    SEG_ASM(STA_X | STA_R, - KERNBASE, 0xFFFFFFFF)      # code segment
    SEG_ASM(STA_W, - KERNBASE, 0xFFFFFFFF)              # data segment
__gdtdesc:
    .word 0x17                                          # sizeof(__gdt) - 1
    .long REALLOC(__gdt)

# KERNBASE            0xC0000000
#define SEG_ASM(type,base,lim)                                  \
    .word (((lim) >> 12) & 0xffff), ((base) & 0xffff);          \
    .byte (((base) >> 16) & 0xff), (0x90 | (type)),             \
        (0xC0 | (((lim) >> 28) & 0xf)), (((base) >> 24) & 0xff)