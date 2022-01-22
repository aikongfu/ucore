.code16

movw $0xb800, %ax
movw %ax, %es

movb $'H', %es:0
movb $0xa, %es:1
movb $'e', %es:2
movb $0xa, %es:3
movb $'l', %es:4
movb $0xa, %es:5
movb $'l', %es:6
movb $0xa, %es:7
movb $'o', %es:8
movb $0xa, %es:9
movb $',', %es:0xA
movb $0xa, %es:0xB
movb $'W', %es:0xC
movb $0xa, %es:0xD
movb $'o', %es:0xE
movb $0xa, %es:0xF
movb $'r', %es:0x10
movb $0xa, %es:0x11
movb $'l', %es:0x12
movb $0xa, %es:0x13
movb $'d', %es:0x14
movb $0xa, %es:0x15

jmp .

.org 510
.word 0xAA55