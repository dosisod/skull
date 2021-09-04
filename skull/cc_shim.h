#define SHIM "echo \'	.file	\"\"\n" \
"	.text\n" \
"	.globl	_strcmp\n" \
"	.type	_strcmp, @function\n" \
"_strcmp:\n" \
".LFB0:\n" \
"	.cfi_startproc\n" \
"	pushq	%%rbp\n" \
"	.cfi_def_cfa_offset 16\n" \
"	.cfi_offset 6, -16\n" \
"	movq	%%rsp, %%rbp\n" \
"	.cfi_def_cfa_register 6\n" \
"	movq	%%rdi, -8(%%rbp)\n" \
"	movq	%%rsi, -16(%%rbp)\n" \
"	jmp	.L2\n" \
".L6:\n" \
"	movq	-8(%%rbp), %%rax\n" \
"	movzbl	(%%rax), %%edx\n" \
"	movq	-16(%%rbp), %%rax\n" \
"	movzbl	(%%rax), %%eax\n" \
"	cmpb	%%al, %%dl\n" \
"	je	.L3\n" \
"	movl	$0, %%eax\n" \
"	jmp	.L4\n" \
".L3:\n" \
"	addq	$1, -8(%%rbp)\n" \
"	addq	$1, -16(%%rbp)\n" \
".L2:\n" \
"	movq	-8(%%rbp), %%rax\n" \
"	movzbl	(%%rax), %%eax\n" \
"	testb	%%al, %%al\n" \
"	je	.L5\n" \
"	movq	-16(%%rbp), %%rax\n" \
"	movzbl	(%%rax), %%eax\n" \
"	testb	%%al, %%al\n" \
"	jne	.L6\n" \
".L5:\n" \
"	movq	-8(%%rbp), %%rax\n" \
"	movzbl	(%%rax), %%eax\n" \
"	testb	%%al, %%al\n" \
"	jne	.L7\n" \
"	movq	-16(%%rbp), %%rax\n" \
"	movzbl	(%%rax), %%eax\n" \
"	testb	%%al, %%al\n" \
"	jne	.L7\n" \
"	movl	$1, %%eax\n" \
"	jmp	.L8\n" \
".L7:\n" \
"	movl	$0, %%eax\n" \
".L8:\n" \
"	andl	$1, %%eax\n" \
".L4:\n" \
"	popq	%%rbp\n" \
"	.cfi_def_cfa 7, 8\n" \
"	ret\n" \
"	.cfi_endproc\n" \
".LFE0:\n" \
"	.size	_strcmp, .-_strcmp\n" \
"	.globl	_float_pow\n" \
"	.type	_float_pow, @function\n" \
"_float_pow:\n" \
".LFB1:\n" \
"	.cfi_startproc\n" \
"	pushq	%%rbp\n" \
"	.cfi_def_cfa_offset 16\n" \
"	.cfi_offset 6, -16\n" \
"	movq	%%rsp, %%rbp\n" \
"	.cfi_def_cfa_register 6\n" \
"	movsd	%%xmm0, -8(%%rbp)\n" \
"	movsd	%%xmm1, -16(%%rbp)\n" \
"	nop\n" \
"	popq	%%rbp\n" \
"	.cfi_def_cfa 7, 8\n" \
"	ret\n" \
"	.cfi_endproc\n" \
".LFE1:\n" \
"	.size	_float_pow, .-_float_pow\n" \
"	.globl	_int_pow\n" \
"	.type	_int_pow, @function\n" \
"_int_pow:\n" \
".LFB2:\n" \
"	.cfi_startproc\n" \
"	pushq	%%rbp\n" \
"	.cfi_def_cfa_offset 16\n" \
"	.cfi_offset 6, -16\n" \
"	movq	%%rsp, %%rbp\n" \
"	.cfi_def_cfa_register 6\n" \
"	movq	%%rdi, -24(%%rbp)\n" \
"	movq	%%rsi, -32(%%rbp)\n" \
"	movq	-24(%%rbp), %%rax\n" \
"	movq	%%rax, -16(%%rbp)\n" \
"	movq	$1, -8(%%rbp)\n" \
"	jmp	.L11\n" \
".L12:\n" \
"	movq	-16(%%rbp), %%rax\n" \
"	imulq	-24(%%rbp), %%rax\n" \
"	movq	%%rax, -16(%%rbp)\n" \
"	addq	$1, -8(%%rbp)\n" \
".L11:\n" \
"	movq	-8(%%rbp), %%rax\n" \
"	cmpq	-32(%%rbp), %%rax\n" \
"	jl	.L12\n" \
"	movq	-16(%%rbp), %%rax\n" \
"	popq	%%rbp\n" \
"	.cfi_def_cfa 7, 8\n" \
"	ret\n" \
"	.cfi_endproc\n" \
".LFE2:\n" \
"	.size	_int_pow, .-_int_pow\n" \
"	.globl	main\n" \
"	.type	main, @function\n" \
"main:\n" \
".LFB3:\n" \
"	.cfi_startproc\n" \
"	pushq	%%rbp\n" \
"	.cfi_def_cfa_offset 16\n" \
"	.cfi_offset 6, -16\n" \
"	movq	%%rsp, %%rbp\n" \
"	.cfi_def_cfa_register 6\n" \
"	call	%s@PLT\n" \
"	popq	%%rbp\n" \
"	.cfi_def_cfa 7, 8\n" \
"	ret\n" \
"	.cfi_endproc\n" \
".LFE3:\n" \
"	.size	main, .-main\n" \
"	.ident	\"GCC: (Ubuntu 8.4.0-1ubuntu1~18.04) 8.4.0\"\n" \
"	.section	.note.GNU-stack,\"\",@progbits\n" \
"\' | cc \"%s\" -o \"%s\" %s -I/usr/include/skull -x assembler - -lm"
