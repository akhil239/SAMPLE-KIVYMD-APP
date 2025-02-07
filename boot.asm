[BITS 16]
[ORG 0x7C00]

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; Enable A20 line
    in al, 0x92
    or al, 2
    out 0x92, al

    ; Load GDT
    lgdt [gdt_descriptor]

    ; Switch to 32-bit protected mode
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp CODE_SEG:init32

[BITS 32]
init32:
    ; Update segment registers
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Enable long mode
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; Enable paging
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ; Jump to 64-bit mode
    jmp CODE64_SEG:init64

[BITS 64]
init64:
    ; Now in 64-bit mode
    mov ax, DATA64_SEG
    mov ds, ax
    mov es, ax
    mov ss, ax

    ; Jump to the kernel
    call 0x100000

halt:
    hlt
    jmp halt

gdt:
    dq 0
gdt_code:
    dw 0xFFFF, 0, 0x9A, 0x00
gdt_data:
    dw 0xFFFF, 0, 0x92, 0x00
gdt_code64:
    dw 0xFFFF, 0, 0x9A, 0x20
gdt_data64:
    dw 0xFFFF, 0, 0x92, 0x20

gdt_descriptor:
    dw (gdt_descriptor - gdt) - 1
    dq gdt

CODE_SEG equ gdt_code - gdt
DATA_SEG equ gdt_data - gdt
CODE64_SEG equ gdt_code64 - gdt
DATA64_SEG equ gdt_data64 - gdt

times 510-($-$$) db 0
dw 0xAA55  ; Boot signature
