    .text

    .global _CALL
    .global _SYSCALL
    
    .balign 4

_CALL:
    mov.l   @r15,r1
    jmp     @r1
    nop

_SYSCALL:
    mov.l   SYSCALL_TABLE,r1
    mov.l   @r15,r0
    jmp     @r1
    nop

    .balign 4

SYSCALL_TABLE: .long 0x80020070
