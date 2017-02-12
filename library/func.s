	.file	"func.c"
	.text
	.globl	float2fix32
	.type	float2fix32, @function
float2fix32:
.LFB62:
	.cfi_startproc
	movss	%xmm0, -4(%rsp)
	cmpb	$0, -1(%rsp)
	jns	.L2
	andb	$127, -1(%rsp)
	movss	-4(%rsp), %xmm0
	cvttss2siq	%xmm0, %rdx
	movzbl	%dl, %eax
	movl	%edx, %edx
	cvtsi2ssq	%rdx, %xmm1
	subss	%xmm1, %xmm0
	addss	.LC0(%rip), %xmm0
	movss	%xmm0, -8(%rsp)
	movl	-8(%rsp), %edx
	andl	$8388607, %edx
	sall	$23, %eax
	orl	%edx, %eax
	negl	%eax
	ret
.L2:
	movss	-4(%rsp), %xmm0
	cvttss2siq	%xmm0, %rdx
	movzbl	%dl, %eax
	movl	%edx, %edx
	cvtsi2ssq	%rdx, %xmm1
	subss	%xmm1, %xmm0
	addss	.LC0(%rip), %xmm0
	movss	%xmm0, -8(%rsp)
	movl	-8(%rsp), %edx
	andl	$8388607, %edx
	sall	$23, %eax
	orl	%edx, %eax
	ret
	.cfi_endproc
.LFE62:
	.size	float2fix32, .-float2fix32
	.globl	float2fix16
	.type	float2fix16, @function
float2fix16:
.LFB63:
	.cfi_startproc
	movss	%xmm0, -4(%rsp)
	cmpb	$0, -1(%rsp)
	jns	.L9
	andb	$127, -1(%rsp)
	movss	-4(%rsp), %xmm0
	cvttss2siq	%xmm0, %rdx
	movl	%edx, %eax
	andl	$7, %eax
	movl	%edx, %edx
	cvtsi2ssq	%rdx, %xmm1
	subss	%xmm1, %xmm0
	addss	.LC0(%rip), %xmm0
	movss	%xmm0, -8(%rsp)
	movl	-8(%rsp), %esi
	shrl	$11, %esi
	movl	%esi, %edx
	andl	$4095, %edx
	sall	$12, %eax
	orl	%edx, %eax
	negl	%eax
	ret
.L9:
	movss	-4(%rsp), %xmm0
	cvttss2siq	%xmm0, %rdx
	movl	%edx, %eax
	andl	$7, %eax
	movl	%edx, %edx
	cvtsi2ssq	%rdx, %xmm1
	subss	%xmm1, %xmm0
	addss	.LC0(%rip), %xmm0
	movss	%xmm0, -8(%rsp)
	movl	-8(%rsp), %ecx
	shrl	$11, %ecx
	movl	%ecx, %edx
	andl	$4095, %edx
	sall	$12, %eax
	orl	%edx, %eax
	ret
	.cfi_endproc
.LFE63:
	.size	float2fix16, .-float2fix16
	.globl	fix2float32
	.type	fix2float32, @function
fix2float32:
.LFB64:
	.cfi_startproc
	movl	$0, -4(%rsp)
	movw	$16256, -2(%rsp)
	movl	%edi, %eax
	shrl	$24, %eax
	testb	%al, %al
	jns	.L16
	negl	%edi
	movl	-4(%rsp), %eax
	movl	%eax, %edx
	orl	%edi, %edx
	andl	$8388607, %edx
	andl	$-8388608, %eax
	orl	%edx, %eax
	movl	%eax, -4(%rsp)
	shrl	$23, %edi
	movzbl	%dil, %edi
	cvtsi2ss	%edi, %xmm0
	addss	-4(%rsp), %xmm0
	subss	.LC0(%rip), %xmm0
	movss	%xmm0, -4(%rsp)
	orb	$-128, -1(%rsp)
	jmp	.L17
.L16:
	movl	-4(%rsp), %eax
	movl	%eax, %edx
	orl	%edi, %edx
	andl	$8388607, %edx
	andl	$-8388608, %eax
	orl	%edx, %eax
	movl	%eax, -4(%rsp)
	shrl	$23, %edi
	movzbl	%dil, %edi
	cvtsi2ss	%edi, %xmm0
	addss	-4(%rsp), %xmm0
	subss	.LC0(%rip), %xmm0
	movss	%xmm0, -4(%rsp)
.L17:
	movss	-4(%rsp), %xmm0
	ret
	.cfi_endproc
.LFE64:
	.size	fix2float32, .-fix2float32
	.globl	fix2float16
	.type	fix2float16, @function
fix2float16:
.LFB65:
	.cfi_startproc
	movl	$0, -4(%rsp)
	movw	$16256, -2(%rsp)
	movl	%edi, %eax
	movzbl	%ah, %eax
	testb	%al, %al
	jns	.L19
	negl	%edi
	movl	%edi, %edx
	andl	$4095, %edx
	sall	$11, %edx
	movl	-4(%rsp), %eax
	movl	%eax, %ecx
	andl	$8388607, %ecx
	orl	%ecx, %edx
	andl	$-8388608, %eax
	orl	%edx, %eax
	movl	%eax, -4(%rsp)
	shrl	$12, %edi
	andl	$7, %edi
	cvtsi2ss	%edi, %xmm0
	addss	-4(%rsp), %xmm0
	subss	.LC0(%rip), %xmm0
	movss	%xmm0, -4(%rsp)
	orb	$-128, -1(%rsp)
	jmp	.L20
.L19:
	movl	%edi, %edx
	andl	$4095, %edx
	sall	$11, %edx
	movl	-4(%rsp), %eax
	movl	%eax, %ecx
	andl	$8388607, %ecx
	orl	%ecx, %edx
	andl	$-8388608, %eax
	orl	%edx, %eax
	movl	%eax, -4(%rsp)
	shrl	$12, %edi
	andl	$7, %edi
	cvtsi2ss	%edi, %xmm0
	addss	-4(%rsp), %xmm0
	subss	.LC0(%rip), %xmm0
	movss	%xmm0, -4(%rsp)
.L20:
	movss	-4(%rsp), %xmm0
	ret
	.cfi_endproc
.LFE65:
	.size	fix2float16, .-fix2float16
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC1:
	.string	"Improper argument numbers\r"
.LC2:
	.string	"fixed"
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC3:
	.string	"float32 to fixed32 conversion initiated..\r"
	.section	.rodata.str1.1
.LC4:
	.string	"fixed-point %f => %x\r\n"
.LC5:
	.string	"fixed-point %f => %d\r\n"
.LC6:
	.string	"Compare %f => %f\r\n"
	.section	.rodata.str1.8
	.align 8
.LC7:
	.string	"float16 to fixed16 conversion initiated..\r"
	.align 8
.LC8:
	.string	"argv[2] is an unacceptable number.\r"
	.align 8
.LC9:
	.string	"Here is the potential options: 32 or 16\r"
	.align 8
.LC10:
	.string	"fixed32 to float32 conversion initiated..\r"
	.section	.rodata.str1.1
.LC11:
	.string	"float %d => %f\r\n"
.LC12:
	.string	"float %x => %f\r\n"
.LC13:
	.string	"Compare %x => %x\r\n"
	.section	.rodata.str1.8
	.align 8
.LC14:
	.string	"fixed16 to float16 conversion initiated..\r"
	.section	.rodata.str1.1
.LC15:
	.string	"fixed-point %d => %f\r\n"
.LC16:
	.string	"fixed-point %x => %f\r\n"
	.section	.rodata.str1.8
	.align 8
.LC17:
	.string	"Wrong. Your options are: fl or fixed\r"
	.text
	.globl	main
	.type	main, @function
main:
.LFB61:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	subq	$24, %rsp
	.cfi_def_cfa_offset 48
	cmpl	$4, %edi
	je	.L22
	movl	$.LC1, %edi
	call	puts
	movl	$1, %edi
	call	exit
.L22:
	movq	%rsi, %rbx
	movq	24(%rsi), %rax
	movl	$.LC2, %edi
	movl	$6, %ecx
	movq	%rax, %rsi
	repz cmpsb
	seta	%cl
	setb	%dl
	cmpb	%dl, %cl
	jne	.L23
	movq	16(%rbx), %rdi
	movl	$10, %edx
	movl	$0, %esi
	call	strtol
	cmpl	$16, %eax
	je	.L25
	cmpl	$32, %eax
	jne	.L33
	movl	$.LC3, %edi
	call	puts
	movq	8(%rbx), %rdi
	movl	$0, %esi
	call	strtod
	cvtsd2ss	%xmm0, %xmm0
	call	float2fix32
	movl	%eax, %ebp
	movq	8(%rbx), %rdi
	movl	$0, %esi
	call	strtod
	movl	%ebp, %edx
	movl	$.LC4, %esi
	movl	$1, %edi
	movl	$1, %eax
	call	__printf_chk
	movq	8(%rbx), %rdi
	movl	$0, %esi
	call	strtod
	movl	%ebp, %edx
	movl	$.LC5, %esi
	movl	$1, %edi
	movl	$1, %eax
	call	__printf_chk
	movl	%ebp, %edi
	call	fix2float32
	movss	%xmm0, 4(%rsp)
	movq	8(%rbx), %rdi
	movl	$0, %esi
	call	strtod
	cvtss2sd	4(%rsp), %xmm1
	movl	$.LC6, %esi
	movl	$1, %edi
	movl	$2, %eax
	call	__printf_chk
	jmp	.L27
.L25:
	movl	$.LC7, %edi
	call	puts
	movq	8(%rbx), %rdi
	movl	$0, %esi
	call	strtod
	cvtsd2ss	%xmm0, %xmm0
	call	float2fix16
	movl	%eax, %ebp
	movq	8(%rbx), %rdi
	movl	$0, %esi
	call	strtod
	movl	%ebp, %edx
	movl	$.LC4, %esi
	movl	$1, %edi
	movl	$1, %eax
	call	__printf_chk
	movq	8(%rbx), %rdi
	movl	$0, %esi
	call	strtod
	movl	%ebp, %edx
	movl	$.LC5, %esi
	movl	$1, %edi
	movl	$1, %eax
	call	__printf_chk
	movl	%ebp, %edi
	call	fix2float16
	movss	%xmm0, 4(%rsp)
	movq	8(%rbx), %rdi
	movl	$0, %esi
	call	strtod
	cvtss2sd	4(%rsp), %xmm1
	movl	$.LC6, %esi
	movl	$1, %edi
	movl	$2, %eax
	call	__printf_chk
	jmp	.L27
.L33:
	movl	$.LC8, %edi
	call	puts
	movl	$.LC9, %edi
	call	puts
	jmp	.L27
.L23:
	cmpb	$102, (%rax)
	jne	.L28
	cmpb	$108, 1(%rax)
	.p2align 4,,4
	jne	.L28
	cmpb	$0, 2(%rax)
	jne	.L28
	movq	16(%rbx), %rdi
	movl	$10, %edx
	movl	$0, %esi
	call	strtol
	cmpl	$16, %eax
	je	.L30
	cmpl	$32, %eax
	jne	.L34
	movl	$.LC10, %edi
	call	puts
	movq	8(%rbx), %rdi
	movl	$10, %edx
	movl	$0, %esi
	call	strtol
	movl	%eax, %edi
	call	fix2float32
	movss	%xmm0, 4(%rsp)
	cvtss2sd	%xmm0, %xmm2
	movsd	%xmm2, 8(%rsp)
	movq	8(%rbx), %rdi
	movl	$10, %edx
	movl	$0, %esi
	call	strtol
	movsd	8(%rsp), %xmm0
	movl	%eax, %edx
	movl	$.LC11, %esi
	movl	$1, %edi
	movl	$1, %eax
	call	__printf_chk
	movq	8(%rbx), %rdi
	movl	$10, %edx
	movl	$0, %esi
	call	strtol
	movsd	8(%rsp), %xmm0
	movl	%eax, %edx
	movl	$.LC12, %esi
	movl	$1, %edi
	movl	$1, %eax
	call	__printf_chk
	movss	4(%rsp), %xmm0
	call	float2fix32
	movl	%eax, %ebp
	movq	8(%rbx), %rdi
	movl	$10, %edx
	movl	$0, %esi
	call	strtol
	movl	%ebp, %ecx
	movl	%eax, %edx
	movl	$.LC13, %esi
	movl	$1, %edi
	movl	$0, %eax
	call	__printf_chk
	jmp	.L27
.L30:
	movl	$.LC14, %edi
	call	puts
	movq	8(%rbx), %rdi
	movl	$10, %edx
	movl	$0, %esi
	call	strtol
	movl	%eax, %edi
	call	fix2float16
	movss	%xmm0, 4(%rsp)
	cvtss2sd	%xmm0, %xmm3
	movsd	%xmm3, 8(%rsp)
	movq	8(%rbx), %rdi
	movl	$10, %edx
	movl	$0, %esi
	call	strtol
	movsd	8(%rsp), %xmm0
	movl	%eax, %edx
	movl	$.LC15, %esi
	movl	$1, %edi
	movl	$1, %eax
	call	__printf_chk
	movq	8(%rbx), %rdi
	movl	$10, %edx
	movl	$0, %esi
	call	strtol
	movsd	8(%rsp), %xmm0
	movl	%eax, %edx
	movl	$.LC16, %esi
	movl	$1, %edi
	movl	$1, %eax
	call	__printf_chk
	movss	4(%rsp), %xmm0
	call	float2fix16
	movl	%eax, %ebp
	movq	8(%rbx), %rdi
	movl	$10, %edx
	movl	$0, %esi
	call	strtol
	movl	%ebp, %ecx
	movl	%eax, %edx
	movl	$.LC13, %esi
	movl	$1, %edi
	movl	$0, %eax
	call	__printf_chk
	jmp	.L27
.L34:
	movl	$.LC8, %edi
	call	puts
	movl	$.LC9, %edi
	call	puts
	jmp	.L27
.L28:
	movl	$.LC17, %edi
	call	puts
.L27:
	movl	$0, %eax
	addq	$24, %rsp
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE61:
	.size	main, .-main
	.section	.rodata.cst4,"aM",@progbits,4
	.align 4
.LC0:
	.long	1065353216
	.ident	"GCC: (Ubuntu 4.8.4-2ubuntu1~14.04.3) 4.8.4"
	.section	.note.GNU-stack,"",@progbits
