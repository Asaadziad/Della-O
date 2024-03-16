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
	movl $69, %esi
	leaq fmt_int(%rip), %rdi
	movl $0, %eax
	callq printf
	movl $0, %eax
	leave
	ret
.type main, @function
.size main, .-main
/* end function main */

.section .note.GNU-stack,"",@progbits
