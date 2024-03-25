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
	pushq %rbx
	pushq %r12
	movl $9, %esi
	movl $16, %edi
	callq add
	movl %eax, %esi
	movl %esi, %r12d
	movl $4, %ebx
.Lbb4:
	subl $1, %ebx
	subq $16, %rsp
	movq %rsp, %rdi
	movb $104, (%rdi)
	movb $101, 1(%rdi)
	movb $108, 2(%rdi)
	movb $108, 3(%rdi)
	movb $111, 4(%rdi)
	movb $32, 5(%rdi)
	movb $119, 6(%rdi)
	movb $111, 7(%rdi)
	movb $114, 8(%rdi)
	movb $108, 9(%rdi)
	movb $100, 10(%rdi)
	movb $0, 11(%rdi)
	callq printf
	cmpl $0, %ebx
	jnz .Lbb4
	movl %r12d, %esi
	leaq fmt_int(%rip), %rdi
	callq printf
	movq %rbp, %rsp
	subq $16, %rsp
	popq %r12
	popq %rbx
	leave
	ret
.type main, @function
.size main, .-main
/* end function main */

.section .note.GNU-stack,"",@progbits
