	.file	"test.c"
	.section	.rodata
.LC0:
	.string	"sizeof myfs_dentry:%d\n"
.LC1:
	.string	"sizeof unsigned long:%d\n"
.LC2:
	.string	"sizeof unsigned int:%d\n"
	.text
.globl main
	.type	main, @function
main:
	leal	4(%esp), %ecx
	andl	$-16, %esp
	pushl	-4(%ecx)
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ecx
	subl	$20, %esp
	movl	$60, 4(%esp)
	movl	$.LC0, (%esp)
	call	printf
	movl	$4, 4(%esp)
	movl	$.LC1, (%esp)
	call	printf
	movl	$4, 4(%esp)
	movl	$.LC2, (%esp)
	call	printf
	addl	$20, %esp
	popl	%ecx
	popl	%ebp
	leal	-4(%ecx), %esp
	ret
	.size	main, .-main
	.ident	"GCC: (SUSE Linux) 4.3.2 [gcc-4_3-branch revision 141291]"
	.section	.comment.SUSE.OPTs,"MS",@progbits,1
	.ascii	"ospwg"
	.section	.note.GNU-stack,"",@progbits
