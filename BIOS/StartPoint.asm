.286
.MODE TINY
extrn _BootMain:near
.code
org 0x7C00       ; Set origin to bootloader start location

main:
    jmp short start
    nop

start:
    cli                 ; Disable interrupts during setup
    mov ax, cs          ; Set up segment registers
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov bp, 0x7E00      ; Safe stack position above bootloader code
    mov sp, 0x7E00
    sti                 ; Enable interrupts again

    call _BootMain      ; Call main boot function
    cli                 ; Disable interrupts

hang:
    hlt                 ; Halt CPU
    jmp hang            ; Infinite loop to prevent returning

    ; Padding to 512 bytes for boot sector
    times 510-($-$$) db 0
    db 0x55, 0xAA       ; Boot sector signature

END main
