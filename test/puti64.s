	.file	"puti64.c"
	.text
	.globl	puti64
	.type	puti64, @function
# function puti64
puti64:
.LFB0:
	.cfi_startproc
	pushq	%rbp                        # store last rbp on stack
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp                  # set rbp to rsp
	.cfi_def_cfa_register 6
	subq	$64, %rsp                   # get 64B stack space for local variables
	movq	%rdi, -56(%rbp)             # num in rdi onto stack for local scope?
	
    # sz = 1;
    movl	$1, -4(%rbp)                # sz = 1
	movl	-4(%rbp), %eax              # sz = 1
	cltq                                # convert long to quad

    # buf[sizeof(buf) - sz - 1] = '\n';
	movl	$31, %edx                   # sizeof(buf)
	subq	%rax, %rdx                  # sz = sizeof(buf) - sz;        buf[30]
	movb	$10, -48(%rbp,%rdx)         # buf[]

# do { ... } while(num);
.L2:
	movq	-56(%rbp), %rcx                 # sz -> rcx
	movabsq	$7378697629483820647, %rdx      # magic number
	movq	%rcx, %rax                      # rcx -> rax
	imulq	%rdx
	sarq	$2, %rdx
	movq	%rcx, %rax
	sarq	$63, %rax
	subq	%rax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	addq	%rax, %rax
	subq	%rax, %rcx
	movq	%rcx, %rdx
	movl	%edx, %eax
	leal	48(%rax), %ecx
	movl	-4(%rbp), %eax
	cltq
	movl	$31, %edx
	subq	%rax, %rdx
	movl	%ecx, %eax
	movb	%al, -48(%rbp,%rdx)
	movq	-56(%rbp), %rcx
	movabsq	$7378697629483820647, %rdx
	movq	%rcx, %rax
	imulq	%rdx
	movq	%rdx, %rax
	sarq	$2, %rax
	sarq	$63, %rcx
	movq	%rcx, %rdx
	subq	%rdx, %rax
	movq	%rax, -56(%rbp)
	addl	$1, -4(%rbp)
	cmpq	$0, -56(%rbp)
	jne	.L2
	movl	-4(%rbp), %eax
	cltq
	movl	-4(%rbp), %edx
	movslq	%edx, %rdx
	movl	$32, %ecx
	subq	%rdx, %rcx
	leaq	-48(%rbp), %rdx
	addq	%rdx, %rcx
	movq	%rax, %rdx
	movq	%rcx, %rsi
	movl	$1, %edi
	call	write@PLT
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	puti64, .-puti64
	.ident	"GCC: (Debian 14.2.0-8) 14.2.0"
	.section	.note.GNU-stack,"",@progbits
