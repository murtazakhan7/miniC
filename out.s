.section .text
.align 4

.globl main
.type main, @function
main:
    push %rbp
    mov %rsp, %rbp
    sub $64, %rsp
    movl $1, %eax
    movl %eax, -4(%rbp)
    movl $2, %eax
    movl %eax, -8(%rbp)
    movl $32, %eax
    movl %eax, -16(%rbp)
    movl -16(%rbp), %eax
    movl %eax, -12(%rbp)
    movl -4(%rbp), %eax
    movl -8(%rbp), %ecx
    addl %ecx, %eax
    movl %eax, -28(%rbp)
    movl -28(%rbp), %eax
    movl %eax, -24(%rbp)
    movl -28(%rbp), %eax
    movl %eax, -36(%rbp)
    movl -36(%rbp), %eax
    movl %eax, -32(%rbp)
    movl -24(%rbp), %eax
    movl -32(%rbp), %ecx
    addl %ecx, %eax
    movl %eax, -48(%rbp)
    movl -48(%rbp), %eax
    movl -12(%rbp), %ecx
    addl %ecx, %eax
    movl %eax, -52(%rbp)
    leaq .LC0(%rip), %rdi
    movl -52(%rbp), %esi
    movl $0, %eax
    call printf
    movl %eax, -56(%rbp)
    movl $0, %eax
    jmp .L_return_main
.L_return_main:
    leave
    ret

.section .rodata
.align 4
.LC0:
    .asciz "%d"

