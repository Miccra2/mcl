	.file	"puti64_2.c"
	.text
	.p2align 4
	.globl	puti64
	.type	puti64, @function
# puti64 call (uint64_t num = rdi)
puti64:
.LFB0:
	.cfi_startproc
	movabsq	$7378697629483820647, %rsi      # magic value for "num % 10" and or "num / 10"
	subq	$40, %rsp                       # local scope variables "char buf[32]" and "int sz" = 40B
	.cfi_def_cfa_offset 48
	movb	$10, 30(%rsp)                   # buf[sizeof(buf) - 2] = '\n';
	movq	%rsp, %r8                       # &buf;
	movq	%rsp, %rcx                      # rcx = &buf;
	.p2align 6
	.p2align 4
	.p2align 3
.L2:
    # num % 10
	movq	%rdi, %rax                      # rax = num;                    // num = rdi
	imulq	%rsi                            # rdx, rax = rax * rsi;         // i128 for handeling overflow
	movq	%rdi, %rax                      # rax = num;
	sarq	$63, %rax                       # rax >>= 63;
	sarq	$2, %rdx                        # rdx >>= 2;
	subq	%rax, %rdx                      # rdx /= rax
	leaq	(%rdx,%rdx,4), %rax             # rax = [rdx+rdx*4]
	addq	%rax, %rax                      # rax *= 2
	subq	%rax, %rdi                      # rdi -= rax
	movq	%rcx, %rax
	subq	$1, %rcx
	addl	$48, %edi
	movb	%dil, 31(%rcx)
	movq	%rdx, %rdi
	testq	%rdx, %rdx
	jne	.L2
	leal	2(%r8), %edx
	movl	$1, %edi
	subl	%eax, %edx
	movslq	%edx, %rdx
	subq	%rdx, %r8
	leaq	32(%r8), %rsi
	call	write@PLT
	addq	$40, %rsp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE0:
	.size	puti64, .-puti64
	.ident	"GCC: (Debian 14.2.0-8) 14.2.0"
	.section	.note.GNU-stack,"",@progbits
