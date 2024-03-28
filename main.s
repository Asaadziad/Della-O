.data
.balign 8
fmt_int:
	.ascii "%d"
	.byte 0
/* end data */

.text
isOdd:
	pushq %rbp
	movq %rsp, %rbp
	movl %edi, %eax
	movl $2, %ecx
	cltd
	idivl %ecx
	movl %edx, %eax
	cmpl $0, %eax
	setz %al
	movzbl %al, %eax
	cmpl $0, %eax
	jnz .Lbb3
	movl $1, %eax
	jmp .Lbb4
.Lbb3:
	movl $0, %eax
.Lbb4:
	leave
	ret
.type isOdd, @function
.size isOdd, .-isOdd
/* end function isOdd */

.text
.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	pushq %rbx
	pushq %r12
	movb $104, -8(%rbp)
	movb $101, -7(%rbp)
	movb $108, -6(%rbp)
	movb $108, -5(%rbp)
	movb $111, -4(%rbp)
	movb $0, -3(%rbp)
	movl $0, %r12d
	movl $100, %ebx
.Lbb7:
	subl $1, %ebx
	callq isOdd
	cmpl $0, %eax
	jz .Lbb10
	movl %r12d, %esi
	leaq fmt_int(%rip), %rdi
	callq printf
.Lbb10:
	subq $16, %rsp
	movq %rsp, %rdi
	movb $104, (%rdi)
	movb $101, 1(%rdi)
	movb $108, 2(%rdi)
	movb $108, 3(%rdi)
	movb $111, 4(%rdi)
	movb $0, 5(%rdi)
	callq printf
	addl $1, %r12d
	cmpl $0, %ebx
	jnz .Lbb7
	leaq -8(%rbp), %rdi
	callq printf
	movq %rbp, %rsp
	subq $32, %rsp
	popq %r12
	popq %rbx
	leave
	ret
.type main, @function
.size main, .-main
/* end function main */

.section .note.GNU-stack,"",@progbits
