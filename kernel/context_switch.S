.section .text
.global context_switch
.type context_switch, @function
context_switch:
    /* rdi = old context, rsi = new context */
    mov %rbx, 0(%rdi)
    mov %rbp, 8(%rdi)
    mov %r12, 16(%rdi)
    mov %r13, 24(%rdi)
    mov %r14, 32(%rdi)
    mov %r15, 40(%rdi)

    mov (%rsp), %rax
    mov %rax, 56(%rdi)
    lea 8(%rsp), %rax
    mov %rax, 48(%rdi)

    mov 0(%rsi), %rbx
    mov 8(%rsi), %rbp
    mov 16(%rsi), %r12
    mov 24(%rsi), %r13
    mov 32(%rsi), %r14
    mov 40(%rsi), %r15
    mov 48(%rsi), %rsp
    mov 56(%rsi), %rax
    jmp *%rax
.size context_switch, . - context_switch
