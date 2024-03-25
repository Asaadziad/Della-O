.data
.balign 8
fmt_int:
	.ascii "%d"
	.byte 0
/* end data */

.text
add:
	pushq %rbp
	movq %rsp, %rbp
	movl %edi, %eax
	addl %esi, %eax
	leave
	ret
.type add, @function
.size add, .-add
/* end function add */

.text
.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $24, %rsp
	pushq %rbx
	movl $9, %esi
	movl $16, %edi
	callq add
	movl %eax, %ebx
	movb $104, -8(%rbp)
	movb $101, -7(%rbp)
	movb $108, -6(%rbp)
	movb $108, -5(%rbp)
	movb $111, -4(%rbp)
	movb $0, -3(%rbp)
	leaq -8(%rbp), %rdi
	callq printf
	movl %ebx, %esi
	leaq fmt_int(%rip), %rdi
	callq printf
	popq %rbx
	leave
	ret
.type main, @function
.size main, .-main
/* end function main */

.section .note.GNU-stack,"",@progbits
