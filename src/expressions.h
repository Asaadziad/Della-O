#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include <string>
#include <memory>
#include <vector>
#include <cstdio>
#include <cstdlib>

typedef enum {
  BINOP_PLUS,
  BINOP_MULT,
  BINOP_ASSIGN,
} BinopType;

class Expr {
  public:  
  virtual ~Expr() = default; 
  virtual void generateCode(FILE* out,int* stack_size) = 0;      
};

class ExprNumber: public Expr {
  private:
    double val;
  public:
    ExprNumber(double val): val(val){};
    double getValue() {return val;}
    virtual void generateCode(FILE* out, int* stack_size) override {
     fprintf(out, "   %%s%d =w copy %d\n", *stack_size, (int)val);    
     *stack_size += 1;
    }
};

class ExprVariable: public Expr {
  std::string name; 
  public:
    ExprVariable(std::string name):name(name){};
    virtual void generateCode(FILE* out,int* stack_size) override { 
      fprintf(out, "%s", name.c_str()); 
      *stack_size += 1;
    } 
};

class ExprBinary: public Expr {
  BinopType op_type;
  std::unique_ptr<Expr> LHS;
  std::unique_ptr<Expr> RHS;
  public:
    ExprBinary(BinopType op_type, std::unique_ptr<Expr> LHS, std::unique_ptr<Expr> RHS):
                op_type(op_type), LHS(std::move(LHS)), RHS(std::move(RHS)) {};
    virtual void generateCode(FILE* out,int* stack_size) override {
      LHS->generateCode(out, stack_size);
      RHS->generateCode(out, stack_size);
      switch(op_type) {
        case BINOP_PLUS:
          fprintf(out,  "   %%s%d =w add %%s%d, %%s%d\n", *stack_size - 2, *stack_size - 2, *stack_size - 1);
          *stack_size -= 1;
          break;
        case BINOP_MULT:
          fprintf(out, "   %%s%d =w mul %%s%d, %%s%d\n", *stack_size - 2, *stack_size - 2, *stack_size - 1);
          *stack_size -= 1;
          break;
        case BINOP_ASSIGN:
                     
          break;
        default:break; 
      }
       
    }
};

class ExprFunCall : public Expr {
  std::string name;
  std::vector<std::unique_ptr<Expr>> args;
  public:
  ExprFunCall(std::string name, std::vector<std::unique_ptr<Expr>> args): name(name),args(std::move(args)) {};
  virtual void generateCode(FILE* out,int* stack_size) override {
    int tmp = *stack_size;
    for(auto& argument: args) {
      argument->generateCode(out, stack_size);
    }
    fprintf(out, "  call $%s(" , name.c_str());
    while(*stack_size > tmp) {
      fprintf(out, "%%s%d, ", *stack_size - 1);
      *stack_size -= 1;
    } 
    fprintf(out, ")\n");
  }
};


class ExprProto {
  std::string name;
  std::vector<std::string> args;
  public:
  ExprProto(std::string name, std::vector<std::string> args): name(name), args(std::move(args)) {};
  const std::string& getName() { return name; }; 
  const std::vector<std::string> getArgs() { return args; };
};


class ExprStmt : public Expr {
  std::unique_ptr<Expr> expr;
  public:
  ExprStmt(std::unique_ptr<Expr> e): expr(std::move(e)) {};
  virtual void generateCode(FILE* out,int* stack_size){
    //expr->generateCode(out, stack_size);
  };
};

class RetStmt: public Expr {
  std::unique_ptr<Expr> expr;
  public:
  RetStmt(std::unique_ptr<Expr> e): expr(std::move(e)) {};
  virtual void generateCode(FILE* out,int* stack_size){
    expr->generateCode(out, stack_size);
    fprintf(out,"   ret %%s%d", *stack_size - 1);
  };
};

class PrintStmt: public Expr {
  std::unique_ptr<Expr> expr;
  public:
  PrintStmt(std::unique_ptr<Expr> e): expr(std::move(e)) {};
  virtual void generateCode(FILE* out,int* stack_size){
  };
};

class FunDeclaration: public Expr {
  std::vector<std::unique_ptr<Expr>> stmts;
  std::unique_ptr<ExprProto>         proto;
  public:
  FunDeclaration(std::vector<std::unique_ptr<Expr>> stmts, std::unique_ptr<ExprProto> proto):
    stmts(std::move(stmts)), proto(std::move(proto)) {};
  virtual void generateCode(FILE* out,int* stack_size){
    fprintf(out, "function w $%s(", proto->getName().c_str()); 
    for(auto& arg: proto->getArgs()) {
      fprintf(out, "%s", arg.c_str());
    }
    fprintf(out, " ){\n@start\n");

    for(auto& stmt: stmts) {
      stmt->generateCode(out, stack_size);
    }
    fprintf(out, "\n}\n");
  };
};

class VarDeclaration: public Expr {
  std::unique_ptr<Expr>         var_exp;
  std::unique_ptr<Expr>         var_val;
  public:
  VarDeclaration(std::unique_ptr<Expr> var_exp, std::unique_ptr<Expr> var_eval): var_exp(std::move(var_exp)), var_val(std::move(var_eval)) {}
  virtual void generateCode(FILE* out,int* stack_size){
  var_val->generateCode(out, stack_size);
  fprintf(out, "    %%");
  var_exp->generateCode(out, stack_size);
  fprintf(out, " =w copy %%s%d", *stack_size - 2);
 };
};

#endif
