[BITS 16]
[ORG 0x7C00]

start:
    mov si, msg_loading
    call print_string

    ; Load kernel from disk (Sector 2 onwards)
    mov ax, 0x1000  ; Load at 0x1000:0000
    mov es, ax
    mov bx, 0x0000
    mov ah, 0x02    ; BIOS read function
    mov al, 10      ; Read 10 sectors
    mov ch, 0       ; Cylinder 0
    mov cl, 2       ; Start from sector 2
    mov dh, 0       ; Head 0
    mov dl, 0       ; Drive 0 (floppy/HDD)
    int 0x13
    jc disk_error

    ; Jump to the C kernel
    jmp 0x1000:0x0000

disk_error:
    mov si, msg_disk_err
    call print_string
    hlt

print_string:
    lodsb
    or al, al
    jz done
    mov ah, 0x0E
    int 0x10
    jmp print_string
done:
    ret

msg_loading db "Loading OS...", 0
msg_disk_err db "Disk Read Error!", 0

times 510 - ($ - $$) db 0  
dw 0xAA55  ; Boot signature
