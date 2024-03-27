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
	movl $3, %edi
	callq isOdd
	cmpl $0, %eax
	jnz .Lbb8
	movl $9, %esi
	leaq fmt_int(%rip), %rdi
	callq printf
	jmp .Lbb9
.Lbb8:
	movl $11, %esi
	leaq fmt_int(%rip), %rdi
	callq printf
.Lbb9:
	leave
	ret
.type main, @function
.size main, .-main
/* end function main */

.section .note.GNU-stack,"",@progbits
