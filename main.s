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
	subq $8, %rsp
	pushq %rbx
	movl $9, %esi
	movl $16, %edi
	callq add
	movl %eax, %ebx
	subq $16, %rsp
	movq %rsp, %rdi
	movb $104, (%rdi)
	movb $101, 1(%rdi)
	movb $108, 2(%rdi)
	movb $108, 3(%rdi)
	movb $111, 4(%rdi)
	movb $98, 5(%rdi)
	movb $111, 6(%rdi)
	movb $121, 7(%rdi)
	movb $0, 8(%rdi)
	callq printf
	movl %ebx, %esi
	leaq fmt_int(%rip), %rdi
	callq printf
	movq %rbp, %rsp
	subq $16, %rsp
	popq %rbx
	leave
	ret
.type main, @function
.size main, .-main
/* end function main */

.section .note.GNU-stack,"",@progbits
