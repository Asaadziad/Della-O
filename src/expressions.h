#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include <string>
#include <memory>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>

typedef enum {
  VOID,
  INT,
  STRING,
  BOOL,
} LType;

typedef enum {
  BINOP_PLUS,
  BINOP_MINUS,
  BINOP_MULT,
  BINOP_DIVIDE,
    
  BINOP_ASSIGN,
} BinopType;

typedef enum {
  EXPR_NUMBER,
  EXPR_VAR,
  EXPR_BINARY,
  
  EXPR_VARDEC,
  EXPR_DEC,
  EXPR_FUNDEC,
  EXPR_STRING,


  EXPR_FUNCALL,
  EXPR_RETURN,
  EXPR_BLOCK,
  EXPR_DECLARATION,
  EXPR_PROGRAM,
} ExprType;

class Expr {
  public:
  virtual ~Expr() = default; 
  virtual ExprType getType() = 0;
  virtual void generateCode(FILE* out, int* stack_size) = 0;
};

class StringExpr: public Expr {
  std::string val;
  public:
  StringExpr(std::string val): val(std::move(val)) {};
  virtual void generateCode(FILE* out, int* stack_size) override {
    size_t len = val.size();
    fprintf(out, "%%A0 =l alloc4 %lu\n", len);
    for(int i= 0 ; i < len; i++) {
        fprintf(out, "%%A%d =l add %%A%d, 1\n", i + 1, i); 
    }
         
    for(int i= 0 ; i < len; i++) {
        fprintf(out, "storeb %d, %%A%d\n",val.c_str()[i], i); 
    }
    fprintf(out, "%%A%lu =l add %%A%lu, 1\n", len, len - 1);
    fprintf(out, "storeb 0, %%A%zu\n", len);
    fprintf(out, "%%s%d =l copy %%A0\n", *stack_size);
    *stack_size += 1;
  };
  virtual ExprType getType() override {
    return EXPR_STRING;
  }
};

class NumberExpr: public Expr { 
  double val;
  public:
    NumberExpr(double val): val(val) {};  
    virtual ExprType getType() override {
      return EXPR_NUMBER;
    };
    virtual void generateCode(FILE* out, int* stack_size) override {
        fprintf(out, "%%s%d =w copy %d\n", *stack_size,(int)val);
        *stack_size += 1;
    };
};

class VarExpr: public Expr {
  std::string id_name;
  LType       var_type;
  public:
  VarExpr(std::string id_name):id_name(id_name), var_type(VOID) {};
  VarExpr(std::string id_name, LType type): id_name(id_name), var_type(type){};
  void setVarType(LType type) { var_type = type; };   
   virtual ExprType getType() override {
      return EXPR_VAR;
    }; 
  virtual void generateCode(FILE* out, int* stack_size) override {
      fprintf(out,"%%%s", id_name.c_str());
      
  };

};

class FunCall: public Expr {
  std::string name;
  std::vector<std::unique_ptr<Expr>> args;
  public:
  FunCall(std::string name, std::vector<std::unique_ptr<Expr>> args): name(name), args(std::move(args)){};
   virtual ExprType getType() override {
      return EXPR_FUNCALL;
    };
  virtual void generateCode(FILE* out, int* stack_size) override {
    std::vector<int> fixed_args;
    for(auto& arg: args) {
      arg->generateCode(out, stack_size);
      fixed_args.push_back(*stack_size);
    }
    fprintf(out, "call $%s(", name.c_str());
    for(int i = 0; i < fixed_args.size(); i++) {
      if(i>0) {
        fprintf(out, ", ");
      }
      fprintf(out, "%%s%d", fixed_args[i]);
    }
    fprintf(out, ")\n");
  };

};

class BinaryExpr : public Expr {
  std::unique_ptr<Expr> lhs;
  std::unique_ptr<Expr> rhs;
  BinopType             op;
  public:
  BinaryExpr(std::unique_ptr<Expr> lhs,
             std::unique_ptr<Expr> rhs,
             BinopType             op):
            lhs(std::move(lhs)), 
            rhs(std::move(rhs)),
            op(op) {};
  
  virtual ExprType getType() override {
      return EXPR_BINARY;
  };
  virtual void generateCode(FILE* out, int* stack_size) override {
    switch(op) {
      case BINOP_PLUS:
        if(lhs->getType() == EXPR_VAR) {
          fprintf(out, "%%s%d =w copy ", *stack_size);
          lhs->generateCode(out, stack_size);
          *stack_size += 1 ;
          fprintf(out,"\n");
        } else {
          lhs->generateCode(out, stack_size);
        }
        
        if(rhs->getType() == EXPR_VAR) {
          fprintf(out, "%%s%d =w copy ", *stack_size);
          rhs->generateCode(out, stack_size);
          fprintf(out,"\n");
          *stack_size += 1 ;
        } else {
          rhs->generateCode(out, stack_size);
        } 
        fprintf(out, "%%s%d =w add %%s%d, %%s%d\n", *stack_size - 2, *stack_size - 2, *stack_size -1); 
        *stack_size -= 1; 
        break;
      case BINOP_MULT:
        if(lhs->getType() == EXPR_VAR) {
          fprintf(out, "%%s%d =w copy ", *stack_size);
          lhs->generateCode(out, stack_size);
          *stack_size += 1 ;
          fprintf(out,"\n");
        } else {
          lhs->generateCode(out, stack_size);
        }
        
        if(rhs->getType() == EXPR_VAR) {
          fprintf(out, "%%s%d =w copy ", *stack_size);
          rhs->generateCode(out, stack_size);
          fprintf(out,"\n");
          *stack_size += 1 ;
        } else {
          rhs->generateCode(out, stack_size);
        } 
        fprintf(out, "%%s%d =w mul %%s%d, %%s%d\n", *stack_size - 2, *stack_size - 2, *stack_size -1); 
        *stack_size -= 1;
        
        break;
      default: break; 
    }
  };

};

class ReturnStatement: public Expr {
  std::unique_ptr<Expr> expr;
  LType                 return_type;
  public:
  ReturnStatement(std::unique_ptr<Expr> expr,
                  LType                 type):
                  expr(std::move(expr)), return_type(type) {};
  
  virtual ExprType getType() override {
    return EXPR_RETURN;
  };
  virtual void generateCode(FILE* out, int* stack_size) override {};

};

class PrintStatement: public Expr {
  std::unique_ptr<Expr> string_expr;
  public:
  PrintStatement(std::unique_ptr<Expr> expr): string_expr(std::move(expr)) {};
  virtual ExprType getType() override {
    return EXPR_FUNCALL;
  };
  virtual void generateCode(FILE* out, int* stack_size) override {
    string_expr->generateCode(out, stack_size);
    fprintf(out, "call $printf(l %%s%d)\n", *stack_size - 1);
  };
};


class Block : public Expr {
  std::map<std::unique_ptr<Expr> ,bool> locals;
  std::vector<std::unique_ptr<Expr>> decls; 
  public:
  Block(std::vector<std::unique_ptr<Expr>> decls): decls(std::move(decls)){};
  
  virtual ExprType getType() override {
    return EXPR_BLOCK;
  };
  virtual void generateCode(FILE* out, int* stack_size) override {
    for(auto& decl : decls) {
      decl->generateCode(out, stack_size);
      
    }
  };

};


//  function name : string
//  arguments : var expression
//  function body : block 
class FunDeclaration: public Expr {
  std::string name;
  std::vector<std::unique_ptr<Expr>> args;
  std::unique_ptr<Expr>              block; 
  LType                              type;
  public:
  FunDeclaration(std::string name,
                std::vector<std::unique_ptr<Expr>> a,
                std::unique_ptr<Expr> block,
                LType type):
                name(std::move(name)),
                args(std::move(a)),
                block(std::move(block)),
                type(type) {};
  
  virtual ExprType getType() override {
    return EXPR_FUNDEC;
  };
  virtual void generateCode(FILE* out, int* stack_size) override {
  
  };

};

class VarDeclaration: public Expr {
  std::unique_ptr<Expr>   var_name;
  std::unique_ptr<Expr>      var_expr;
  ExprType dcl_type;
  public:
  VarDeclaration(std::unique_ptr<Expr> name, std::unique_ptr<Expr> var_expr): var_name(std::move(name)), var_expr(std::move(var_expr)){}; 
  virtual ExprType getType() override {
    return EXPR_DECLARATION;
  };
  virtual void generateCode(FILE* out, int* stack_size) override { 
    var_expr->generateCode(out, stack_size); 
    var_name->generateCode(out, stack_size);
    fprintf(out, " =w copy %%s%d\n", *stack_size - 1);
  };

};

class Program: public Expr {
  std::vector<std::unique_ptr<Expr>> dcls;
  public:
  Program(std::vector<std::unique_ptr<Expr>> exprs): dcls(std::move(exprs)){};
  virtual ExprType getType() override {
    return EXPR_PROGRAM;
  };
  virtual void generateCode(FILE* out, int* stack_size) override {
    for(auto& dcl : dcls) {
      dcl->generateCode( out, stack_size);
    }
  };

};
#endif
