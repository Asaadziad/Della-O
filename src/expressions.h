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

typedef unsigned long long u64;
typedef unsigned long      u32;
typedef unsigned short     u16;
typedef unsigned char      u8;
typedef signed long long   s64;
typedef signed int         s32;
typedef signed short       s16;
typedef signed char        s8;


typedef enum {
  TYPE_INT,
  TYPE_STRING,
  TYPE_BOOLEAN,
} VarType;

class ExprVariable: public Expr {
  std::string name;
  VarType     var_type;
  public:
    ExprVariable(std::string name, VarType type):name(name), var_type(type) {};
    virtual void generateCode(FILE* out,int* stack_size) override { 
     fprintf(out, "Hello"); 
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

class ExprFunDec {
  std::unique_ptr<ExprProto> proto;
  std::unique_ptr<Expr>      body;
  public:
  ExprFunDec(std::unique_ptr<ExprProto> proto, std::unique_ptr<Expr> body): 
    proto(std::move(proto)), body(std::move(body)){};
  void generateCode(FILE* out) {
    fprintf(out, "function w $%s(", proto->getName().c_str());
    for(auto& argName : proto->getArgs()) {
      fprintf(out, " %s,", argName.c_str());
    }
    fprintf(out, "){\n@start\n");
    int stack_size = 0;
    body->generateCode(out, &stack_size);
    fprintf(out, "  ret %%s%d\n", stack_size - 1);
    fprintf(out, "}\n"); 
  };
};

#endif
