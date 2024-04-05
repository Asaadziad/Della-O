.data
.balign 8
fmt:
	.ascii "hello"
	.byte 0
/* end data */

.text
.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	movl $5, %edx
	leaq fmt(%rip), %rsi
	movl $1, %edi
	callq write
	leave
	ret
.type main, @function
.size main, .-main
/* end function main */

.section .note.GNU-stack,"",@progbits
