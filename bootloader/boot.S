/*
 * Multiboot-compliant 64-bit loader for jinOS.
 */

.set ALIGN,    1<<0
.set MEMINFO,  1<<1
.set MAGIC,    0x1BADB002
.set FLAGS,    ALIGN | MEMINFO
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .text
.code32
.global _start
.type _start,@function
_start:
    cli
    mov $stack_top, %esp
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %ss
    mov %ax, %fs
    mov %ax, %gs

    call setup_paging
    call enter_long_mode

hang32:
    hlt
    jmp hang32

setup_paging:
    mov $p4_table, %eax
    mov %eax, %cr3

    mov %cr4, %eax
    or $0x20, %eax          # Enable PAE
    mov %eax, %cr4

    mov $0xC0000080, %ecx
    rdmsr
    or $0x00000100, %eax    # Enable LME in EFER
    wrmsr

    ret

enter_long_mode:
    lgdt gdt_descriptor

    mov %cr0, %eax
    or $0x80000001, %eax    # Set PG and PE
    mov %eax, %cr0

    ljmp $0x08, $long_mode_entry

.size _start, . - _start

.code64
.extern kernel_main

long_mode_entry:
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %ss
    mov %ax, %fs
    mov %ax, %gs

    mov $long_mode_stack_top, %rsp
    call kernel_main

halt:
    hlt
    jmp halt

.section .data
.align 16
gdt64:
    .quad 0x0000000000000000
    .quad 0x00AF9A000000FFFF
    .quad 0x00AF92000000FFFF
gdt_descriptor:
    .word gdt64_end - gdt64 - 1
    .long gdt64
    .long 0x00000000
gdt64_end:

.align 4096
p4_table:
    .quad p3_table + 0x03
    .fill 511,8,0

.align 4096
p3_table:
    .quad p2_table + 0x03
    .fill 511,8,0

.align 4096
p2_table:
    .set i,0
.rept 512
    .quad (i << 21) + 0x83
    .set i, i+1
.endr

.section .bss,"aw",@nobits
.align 16
stack_space:
    .skip 4096
stack_top:

.align 16
long_mode_stack:
    .skip 8192
long_mode_stack_top:
