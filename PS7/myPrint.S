	.text
	.global _start
_start:
	mov	$1, %rax
	mov	$1, %rdi
	mov	$message, %rsi
	mov	$12, %rdx
	syscall
	mov $60, %rax
	mov $5, %rdi
	syscall
message:
	.ascii "hello world\n"
