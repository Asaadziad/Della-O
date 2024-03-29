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
	pushq %rbx
	pushq %r12
	movl $0, %r12d
	movl $5, %ebx
.Lbb7:
	subl $1, %ebx
	movl %r12d, %edi
	callq isOdd
	cmpl $0, %eax
	jz .Lbb10
	movl %r12d, %esi
	leaq fmt_int(%rip), %rdi
	callq printf
.Lbb10:
	addl $1, %r12d
	cmpl $0, %ebx
	jnz .Lbb7
	popq %r12
	popq %rbx
	leave
	ret
.type main, @function
.size main, .-main
/* end function main */

.section .note.GNU-stack,"",@progbits
