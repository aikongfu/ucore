#ifndef __KERN_MM_MEMLAYOUT_H__
#define __KERN_MM_MEMLAYOUT_H__

/* This file contains the definitions for memory management in our OS. */

/* global segment number */
#define SEG_KTEXT    1
#define SEG_KDATA    2
#define SEG_UTEXT    3
#define SEG_UDATA    4
#define SEG_TSS      5

/* global descriptor numbers */
// (1 << 3) = 8     = 1000
#define GD_KTEXT    ((SEG_KTEXT) << 3)        // kernel text
// (2 << 3) = 16    = 00010000
#define GD_KDATA    ((SEG_KDATA) << 3)        // kernel data
// (3 << 3) = 24    = 00011000
#define GD_UTEXT    ((SEG_UTEXT) << 3)        // user text
// (4 << 3) = 32    = 00100000
#define GD_UDATA    ((SEG_UDATA) << 3)        // user data
// (5 << 3) = 40    = 00101000
#define GD_TSS        ((SEG_TSS) << 3)        // task segment selector

#define DPL_KERNEL    (0)
#define DPL_USER    (3)

// 15                                                 3    2        0
// +--------------------------------------------------+----+--------+
// |          Index                                   | TI |   RPL  |
// +--------------------------------------------------+----+--------+
// 
// TI = Table Indicator: 0 = GDT, 1 = LDT

// (1 << 3) | 0 = 8  = 0x08 = 00001000 = 00000000 00001|0|00 -> Index = 1, GDT, RPL = 0 
#define KERNEL_CS    ((GD_KTEXT) | DPL_KERNEL)
// (2 << 3) | 0 = 16 = 0x10 = 00010000 = 00000000 00010|0|00 -> Index = 2, GDT, RPL = 0 
#define KERNEL_DS    ((GD_KDATA) | DPL_KERNEL)
// (3 << 3) | 3 = 27 = 0x1b = 00011011 = 00000000 00011|0|11 -> Index = 3, GDT, RPL = 3 
#define USER_CS        ((GD_UTEXT) | DPL_USER)
// (4 << 3) | 3 = 35 = 0x23 = 00100011 = 00000000 00100|0|11 -> Index = 4, GDT, RPL = 3 
#define USER_DS        ((GD_UDATA) | DPL_USER)

#endif /* !__KERN_MM_MEMLAYOUT_H__ */

