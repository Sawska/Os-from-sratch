.286
.mode TINY
extrn _BootMain:near
.code 
org 0700h
main:
    jmp short start
    nop

start:
    cli
    mov ax,cs
    mov ds,ax
    mov es,ax
    mov ss,ax
    mov bp,7c00h
    mov sp,7c00h
    sti

    call _BootMain
    ret

    END main
