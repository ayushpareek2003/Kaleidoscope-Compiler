#include ".\inc\llvm_code_Gen.h"


#include <iostream>

Value *LogErrorV(const char *Str) {
    std::cerr << "Error: " << Str << std::endl;
    return nullptr;
}

Function *getFunction(std::string Name) {

    if (auto *F = TheModule->getFunction(Name)) {
        return F;
    }

    auto FI = FunctionProtos.find(Name);
    if (FI != FunctionProtos.end()) {
        return FI->second->codegen();
    }

    return nullptr;
}

Value *NumberExprAST::codegen() {
    return ConstantFP::get(*TheContext, APFloat(Val));
}

Value *VariableExprAST::codegen() {
    Value *V = NamedValues[Name];
    if (!V) {
        LogErrorV("Unknown variable name");
    }
    return V;
}

Value *BinaryExprAST::codegen() {
    Value *L = LHS->codegen();
    Value *R = RHS->codegen();
    if (!L || !R) return nullptr;

    switch (Op) {
    case '+':
        return Builder->CreateFAdd(L, R, "addtmp");
    case '-':
        return Builder->CreateFSub(L, R, "subtmp");
    case '*':
        return Builder->CreateFMul(L, R, "multmp");
    case '<':
        L = Builder->CreateFCmpULT(L, R, "cmptmp");
        return Builder->CreateUIToFP(L, Type::getDoubleTy(*TheContext), "booltmp");
    default:
        return LogErrorV("invalid binary operator");
    }
}

Function *PrototypeAST::codegen() {
    std::vector<Type *> Doubles(Args.size(), Type::getDoubleTy(*TheContext));
    FunctionType *FT = FunctionType::get(Type::getDoubleTy(*TheContext), Doubles, false);
    Function *F = Function::Create(FT, Function::ExternalLinkage, Name, TheModule.get());

    unsigned Idx = 0;
    for (auto &Arg : F->args()) {
        Arg.setName(Args[Idx++]);
    }

    return F;
}

Function *FunctionAST::codegen() {
    Function *TheFunction = getFunction(Proto->getName());

    if (!TheFunction) {
        TheFunction = Proto->codegen();
    }

    if (!TheFunction) return nullptr;

    BasicBlock *BB = BasicBlock::Create(*TheContext, "entry", TheFunction);
    Builder->SetInsertPoint(BB);

    NamedValues.clear();
    for (auto &Arg : TheFunction->args()) {
        NamedValues[std::string(Arg.getName())] = &Arg;
    }

    if (Value *RetVal = Body->codegen()) {
        Builder->CreateRet(RetVal);
        verifyFunction(*TheFunction);
        return TheFunction;
    }

    TheFunction->eraseFromParent();
    return nullptr;
}

Value *ForExprAST::codegen() {

    Value *StartVal = Start->codegen();
    if (!StartVal)
      return nullptr;
  

    Function *TheFunction = Builder->GetInsertBlock()->getParent();
    BasicBlock *PreheaderBB = Builder->GetInsertBlock();
    BasicBlock *LoopBB = BasicBlock::Create(*TheContext, "loop", TheFunction);
  
 
    Builder->CreateBr(LoopBB);
 
    Builder->SetInsertPoint(LoopBB);
  
    PHINode *Variable =
        Builder->CreatePHI(Type::getDoubleTy(*TheContext), 2, VarName);
    Variable->addIncoming(StartVal, PreheaderBB);

    Value *OldVal = NamedValues[VarName];
    NamedValues[VarName] = Variable;
  

    if (!Body->codegen())
      return nullptr;
  
    // Emit the step value.
    Value *StepVal = nullptr;
    if (Step) {
      StepVal = Step->codegen();
      if (!StepVal)
        return nullptr;
    } else {

      StepVal = ConstantFP::get(*TheContext, APFloat(1.0));
    }
  
    Value *NextVar = Builder->CreateFAdd(Variable, StepVal, "nextvar");

    Value *EndCond = End->codegen();
    if (!EndCond)
      return nullptr;
 
    EndCond = Builder->CreateFCmpONE(
        EndCond, ConstantFP::get(*TheContext, APFloat(0.0)), "loopcond");

    BasicBlock *LoopEndBB = Builder->GetInsertBlock();
    BasicBlock *AfterBB =
        BasicBlock::Create(*TheContext, "afterloop", TheFunction);

    Builder->CreateCondBr(EndCond, LoopBB, AfterBB);
  

    Builder->SetInsertPoint(AfterBB);
  
    Variable->addIncoming(NextVar, LoopEndBB);
  

    if (OldVal)
      NamedValues[VarName] = OldVal;
    else
      NamedValues.erase(VarName);
  

    return Constant::getNullValue(Type::getDoubleTy(*TheContext));
  }


  Value *IfExprAST::codegen() {
    Value *CondV = Cond->codegen();
    if (!CondV)
      return nullptr;
  
  
    CondV = Builder->CreateFCmpONE(
        CondV, ConstantFP::get(*TheContext, APFloat(0.0)), "ifcond");
  
    Function *TheFunction = Builder->GetInsertBlock()->getParent();
  

    BasicBlock *ThenBB = BasicBlock::Create(*TheContext, "then", TheFunction);
    BasicBlock *ElseBB = BasicBlock::Create(*TheContext, "else");
    BasicBlock *MergeBB = BasicBlock::Create(*TheContext, "ifcont");
  
    Builder->CreateCondBr(CondV, ThenBB, ElseBB);
  

    Builder->SetInsertPoint(ThenBB);
  
    Value *ThenV = Then->codegen();
    if (!ThenV)
      return nullptr;
  
    Builder->CreateBr(MergeBB);

    ThenBB = Builder->GetInsertBlock();
  

    TheFunction->insert(TheFunction->end(), ElseBB);
    Builder->SetInsertPoint(ElseBB);
  
    Value *ElseV = Else->codegen();
    if (!ElseV)
      return nullptr;
  
    Builder->CreateBr(MergeBB);

    ElseBB = Builder->GetInsertBlock();
  
 
    TheFunction->insert(TheFunction->end(), MergeBB);
    Builder->SetInsertPoint(MergeBB);
    PHINode *PN = Builder->CreatePHI(Type::getDoubleTy(*TheContext), 2, "iftmp");
  
    PN->addIncoming(ThenV, ThenBB);
    PN->addIncoming(ElseV, ElseBB);
    return PN;
  }