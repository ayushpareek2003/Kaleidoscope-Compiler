#include <memory>
#include <string>
#include <vector>
#include "lexer.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Function.h"

using namespace llvm;

class ExpressionAST {
public:
  virtual ~ExpressionAST() = default;
  virtual Value *codegen() = 0;
};

class NumberExpressionAST : public ExpressionAST {
  double const value;
public:
  NumberExpressionAST(double const value) : value(value) { }
  double const getValue() const { return value; }
  Value *codegen() override;
};

class VariableExpressionAST : public ExpressionAST {
  std::string const name;
public:
  VariableExpressionAST(std::string const& name) : name(name) { }
  std::string const& getName() const { return name; }
  Value *codegen() override;
};

class UnaryExpressionAST : public ExpressionAST {
  char Opcode;
  std::unique_ptr<ExpressionAST> Operand;
public:
  UnaryExpressionAST(char Opcode, std::unique_ptr<ExpressionAST> Operand)
      : Opcode(Opcode), Operand(std::move(Operand)) {}
  Value *codegen() override;
};

class BinaryExpressionAST : public ExpressionAST {
  char const op;
  std::unique_ptr<ExpressionAST> lhs, rhs;
public:
  BinaryExpressionAST(char op, std::unique_ptr<ExpressionAST> lhs, std::unique_ptr<ExpressionAST> rhs)
      : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
  char getOp() const { return op; }
  ExpressionAST const* getLHS() const { return lhs.get(); }
  ExpressionAST const* getRHS() const { return rhs.get(); }
  Value *codegen() override;
};

class CallExpressionAST : public ExpressionAST {
  std::string callee;
  std::vector<std::unique_ptr<ExpressionAST>> args;
public:
  CallExpressionAST(std::string const& callee, std::vector<std::unique_ptr<ExpressionAST>> args)
      : callee(callee), args(std::move(args)) {}
  std::string const& getCallee() const { return callee; }
  std::vector<std::unique_ptr<ExpressionAST>> const& getArgs() const { return args; }
  Value *codegen() override;
};

class IfExpressionAST : public ExpressionAST {
  std::unique_ptr<ExpressionAST> Cond, Then, Else;
public:
  IfExpressionAST(std::unique_ptr<ExpressionAST> Cond, std::unique_ptr<ExpressionAST> Then,
                  std::unique_ptr<ExpressionAST> Else)
      : Cond(std::move(Cond)), Then(std::move(Then)), Else(std::move(Else)) {}
  Value *codegen() override;
};

class ForExpressionAST : public ExpressionAST {
  std::string VarName;
  std::unique_ptr<ExpressionAST> Start, End, Step, Body;
public:
  ForExpressionAST(const std::string &VarName, std::unique_ptr<ExpressionAST> Start,
                   std::unique_ptr<ExpressionAST> End, std::unique_ptr<ExpressionAST> Step,
                   std::unique_ptr<ExpressionAST> Body)
      : VarName(VarName), Start(std::move(Start)), End(std::move(End)),
        Step(std::move(Step)), Body(std::move(Body)) {}
  Value *codegen() override;
};

class PrototypeAST {
  std::string name;
  std::vector<std::string> args;
  bool IsOperator;
  unsigned Precedence;
public:
  PrototypeAST(const std::string &Name, std::vector<std::string> Args,
               bool IsOperator = false, unsigned Prec = 0)
      : name(Name), args(std::move(Args)), IsOperator(IsOperator),
        Precedence(Prec) {}
  Function *codegen();
  const std::string &getName() const { return name; }
  bool isUnaryOp() const { return IsOperator && args.size() == 1; }
  bool isBinaryOp() const { return IsOperator && args.size() == 2; }
  char getOperatorName() const { return name.back(); }
  unsigned getBinaryPrecedence() const { return Precedence; }
};

class FunctionAST {
  std::unique_ptr<PrototypeAST> prototype;
  std::unique_ptr<ExpressionAST> body;
public:
  FunctionAST(std::unique_ptr<PrototypeAST> prototype, std::unique_ptr<ExpressionAST> body)
      : prototype(std::move(prototype)), body(std::move(body)) {}
  PrototypeAST const* getPrototype() const { return prototype.get(); }
  ExpressionAST const* getBody() const { return body.get(); }
  Function *codegen();
};
