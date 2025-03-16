// llvm_codegen.h
#ifndef LLVM_CODEGEN_H
#define LLVM_CODEGEN_H

#include <map>
#include <memory>
#include <string>
#include "llvm/IR/Value.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/BasicBlock.h"


using namespace llvm;


static std::unique_ptr<LLVMContext> TheContext;
static std::unique_ptr<Module> TheModule;
static std::unique_ptr<IRBuilder<>> Builder;
static std::map<std::string, Value *> NamedValues;
static std::unique_ptr<KaleidoscopeJIT> TheJIT;
static std::unique_ptr<FunctionPassManager> TheFPM;
static std::unique_ptr<LoopAnalysisManager> TheLAM;
static std::unique_ptr<FunctionAnalysisManager> TheFAM;
static std::unique_ptr<CGSCCAnalysisManager> TheCGAM;
static std::unique_ptr<ModuleAnalysisManager> TheMAM;
static std::unique_ptr<PassInstrumentationCallbacks> ThePIC;
static std::unique_ptr<StandardInstrumentations> TheSI;
static std::map<std::string, std::unique_ptr<PrototypeAST>> FunctionProtos;
static ExitOnError ExitOnErr;

// Forward declarations
class PrototypeAST;
class NumberExprAST;
class VariableExprAST;
class BinaryExprAST;
class ForExprAST;
class IfExprAST;
class FunctionAST;


// Function Declarations
Value *LogErrorV(const char *Str);
Function *getFunction(std::string Name);

// AST Class Methods
Value *NumberExprAST_codegen();
Value *VariableExprAST_codegen();
Value *BinaryExprAST_codegen();
Function *PrototypeAST_codegen();
Function *FunctionAST_codegen();
Value *ForExprAST_codegen();
Value *IfExprAST_codegen();

#endif // LLVM_CODEGEN_H
