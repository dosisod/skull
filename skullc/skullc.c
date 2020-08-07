#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include <llvm-c/Core.h>

#include "skull/common/malloc.h"
#include "skull/eval/eval_integer.h"
#include "skull/eval/repl.h"
#include "skull/parse/ast/node.h"

int main(void) {
	LLVMContextRef ctx = LLVMContextCreate();
	LLVMModuleRef main_module = LLVMModuleCreateWithNameInContext("main_module", ctx);

	LLVMTypeRef main_func_type = LLVMFunctionType(
		LLVMInt64Type(),
		NULL,
		0,
		false
	);

	LLVMValueRef main_func = LLVMAddFunction(
		main_module,
		"main",
		main_func_type
	);

	LLVMBasicBlockRef block = LLVMAppendBasicBlockInContext(
		ctx,
		main_func,
		"entry"
	);

	LLVMBuilderRef builder = LLVMCreateBuilderInContext(ctx);

	LLVMPositionBuilderAtEnd(
		builder,
		block
	);

	errno=0;
	FILE *fd=fopen("main.sk", "re");
	if (fd==NULL) {
		puts("error opening file");
		return 1;
	}

	char *str=malloc(sizeof(char) * REPL_MAX_LINE_LEN);
	DIE_IF_MALLOC_FAILS(str);

	size_t offset=0;
	int c=getc(fd);

	while (c!=EOF) {
		//read char by char until we need to reallocate more memory
		if (offset!=0 && ((offset + 1) % REPL_MAX_LINE_LEN)==0) {
			char *new_str=realloc(str, sizeof(char) * (offset + REPL_MAX_LINE_LEN));
			DIE_IF_MALLOC_FAILS(new_str);

			str=new_str;
		}
		str[offset]=(char)c;
		offset++;

		c=getc(fd);
	}
	str[offset]='\0';

	char32_t *file_str=mbstoc32s(str);
	free(str);

	ast_node_t *node=make_ast_tree(file_str);
	free(file_str);

	if (node->node_type==AST_NODE_RETURN) {
		const char32_t *error = NULL;
		int64_t *num = eval_integer(node->token->next, &error);

		LLVMBuildRet(
			builder,
			LLVMConstInt(
				LLVMInt64TypeInContext(ctx),
				(unsigned long long)*num,
				true
			)
		);
	}

	char *err = NULL;
	LLVMBool status = LLVMPrintModuleToFile(
		main_module,
		".tmp.ll",
		&err
	);

	if (err!=NULL || status) {
		puts("error occurred!");
	}

	LLVMDisposeMessage(err);
	LLVMContextDispose(ctx);
	return 0;
}