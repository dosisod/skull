#include "llvm/ADT/APInt.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_ostream.h"

int main() {
	llvm::LLVMContext llvm_ctx;
	llvm::IRBuilder<> builder(llvm_ctx);
	auto module = std::make_unique<llvm::Module>("main_module", llvm_ctx);

	auto f = llvm::Function::Create(
		llvm::FunctionType::get(
			llvm::Type::getInt64Ty(llvm_ctx),
			{},
			false
		),
		llvm::Function::ExternalLinkage,
		"main"
	);

	auto block = llvm::BasicBlock::Create(
		llvm_ctx,
		"",
		f
	);

	builder.SetInsertPoint(block);

	auto num = llvm::ConstantInt::get(llvm_ctx, llvm::APInt(64, 1, true));
	builder.CreateRet(num);

	f->print(llvm::outs(), nullptr);
	module->print(llvm::outs(), nullptr);
	return 0;
}