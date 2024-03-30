.data
.balign 8
fmt_int:
	.ascii "%d"
	.byte 0
/* end data */

.text
.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movb $104, -12(%rbp)
	movb $101, -11(%rbp)
	movb $108, -10(%rbp)
	movb $108, -9(%rbp)
	movb $111, -8(%rbp)
	movb $32, -7(%rbp)
	movb $119, -6(%rbp)
	movb $111, -5(%rbp)
	movb $114, -4(%rbp)
	movb $108, -3(%rbp)
	movb $100, -2(%rbp)
	leaq -12(%rbp), %rdi
	callq printf
	leave
	ret
.type main, @function
.size main, .-main
/* end function main */

.section .note.GNU-stack,"",@progbits
