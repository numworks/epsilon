.text

.global _collect_registers

_collect_registers:
        pushq   %r15
        pushq   %r14
        pushq   %r13
        pushq   %r12
        pushq   %rbp
        pushq   %rbx
        movq    %rbx, (%rdi)
        movq    %rbp, 8(%rdi)
        movq    %r12, 16(%rdi)
        popq    %rbx
        popq    %rbp
        popq    %r12
        movq    %r13, 24(%rdi)
        movq    %r14, 32(%rdi)
        popq    %r13
        movq    %r15, 40(%rdi)
        popq    %r14
        popq    %r15
        movq    %rsp, %rax
        ret
