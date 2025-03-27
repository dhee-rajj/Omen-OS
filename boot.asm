ORG 0x7C00  ; Corrected origin
BITS 16

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

_start: 
    jmp short start
    nop
 times 33 db 0

start:
    jmp 0:step2  ; Jump correctly to 0x7C00-based segment

step2:
    cli             ; Disable interrupts
    mov ax, 0x00
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti             ; Enable interrupts

    ;Load second sector (For future expansion)
    mov ah, 2
    mov al, 1
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov bx, buffer
    int 0x13
    jc error

    mov si, buffer
    call print

.load_protected:
    cli
    lgdt [gdt_descriptor]  ; Load GDT
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp dword CODE_SEG:load32  ; FAR jump to 32-bit mode

; GDT - Global Descriptor Table
gdt_start:
gdt_null:
    dd 0x0
    dd 0x0

gdt_code:
    dw 0xFFFF   ; Limit (low 16 bits)
    dw 0x0000   ; Base (low 16 bits)
    db 0x00     ; Base (middle 8 bits)
    db 0x9A     ; Access byte (Exec, Read, Present)
    db 11001111b ; Flags (4-bit) + Limit (high 4 bits)
    db 0x00     ; Base (high 8 bits)

gdt_data:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x92     ; Data segment (Read/Write, Present)
    db 11001111b
    db 0x00

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Size of GDT
    dd gdt_start  ; Base address of GDT

error:
    mov si, error_msg
    call print
    jmp $

print:
    lodsb
    cmp al, 0
    je .done
    call print_char
    jmp print
.done:
    ret

print_char:
    mov ah, 0x0E
    int 0x10
    ret

error_msg: db 'Error Opening Sector', 0

buffer:

[BITS 32]
load32:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov ebp, 0x00200000
    mov esp, ebp
    jmp $  ; Hang in protected mode

times 510-($ - $$) db 0
dw 0xAA55  ; Boot sector signature
