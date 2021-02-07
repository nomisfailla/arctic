#include <iostream>

#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Transforms/Utils.h>

#include <lld/Common/Driver.h>

int main(int argc, const char** argv)
{
	// Initialize.
	auto context = new llvm::LLVMContext();
	auto builder = new llvm::IRBuilder<>(*context);
	auto module = new llvm::Module("main", *context);
	auto triple = llvm::sys::getDefaultTargetTriple();

	{ // Initialize target.
		module->setTargetTriple(triple);

		llvm::InitializeAllTargetInfos();
		llvm::InitializeAllTargets();
		llvm::InitializeAllTargetMCs();
		llvm::InitializeAllAsmParsers();
		llvm::InitializeAllAsmPrinters();
	}

	{ // Create a function.
		auto funcType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context), std::vector<llvm::Type*> {}, false);
		auto func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "main", module);
		auto entry = llvm::BasicBlock::Create(*context, "entry", func);
		builder->SetInsertPoint(entry);
		auto value = llvm::ConstantInt::getSigned(llvm::Type::getInt32Ty(*context), 4);
		builder->CreateRet(value);
	}

	{ // Do optimizations.
		auto fpm = new llvm::legacy::FunctionPassManager(module);

		// Promote allocas to registers.
		fpm->add(llvm::createPromoteMemoryToRegisterPass());
		// Do simple "peephole" optimizations
		fpm->add(llvm::createInstructionCombiningPass());
		// Reassociate expressions.
		fpm->add(llvm::createReassociatePass());
		// Eliminate Common SubExpressions.
		fpm->add(llvm::createGVNPass());
		// Simplify the control flow graph (deleting unreachable blocks etc).
		fpm->add(llvm::createCFGSimplificationPass());
		
		fpm->doInitialization();
		
		fpm->run(*(module->getFunction("main")));
	}

	{ // Dump object file.
		std::string error;
		auto target = llvm::TargetRegistry::lookupTarget(triple, error);
		
		auto cpu = "generic";
		auto features = "";

		llvm::TargetOptions opt;
		auto rm = llvm::Optional<llvm::Reloc::Model>();
		auto machine = target->createTargetMachine(triple, cpu, features, opt, rm);

		auto filename = "output.o";
		std::error_code ec;
		llvm::raw_fd_ostream dest(filename, ec, llvm::sys::fs::OF_None);

		llvm::legacy::PassManager pass;
		machine->addPassesToEmitFile(pass, dest, nullptr, llvm::CGFT_ObjectFile);

		pass.run(*module);
		dest.flush();
	}

	// lld::elf::link(llvm::ArrayRef<const char*>(argv, argc), false, llvm::outs(), llvm::errs());
}
