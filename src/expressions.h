#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include <string>
#include <memory>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <variant>


typedef enum {
  VOID,
  INT,
} LType;

typedef enum {
  BINOP_PLUS,
  BINOP_MULT,
  BINOP_ASSIGN,
} BinopType;

typedef enum {
  EXPR_NUMBER,
  EXPR_VAR,
  EXPR_BINARY,
  
  EXPR_VARDEC,
  EXPR_DEC,
  EXPR_FUNDEC,
  
  EXPR_FUNCALL,
  EXPR_RETURN,

  EXPR_PROGRAM,
} ExprType;

class Expr {
  public:
  virtual ~Expr() = default;
  virtual void generateCode() = 0;
  virtual ExprType getType() = 0;
};



class NumberExpr: public Expr { 
   double val;
  public:
    NumberExpr(double val): val(val) {}; 
    virtual void generateCode() override {
         printf("%d", (int)val); 
    };
    virtual ExprType getType() override {
      return EXPR_NUMBER;
    };
};

class VarExpr: public Expr {
  std::string id_name;
  LType       var_type;
  public:
  VarExpr(std::string id_name):id_name(id_name){};
  void setVarType(LType type) { var_type = type;};
  virtual void generateCode() override {
      printf("%%%s", id_name.c_str()); 
  };
   virtual ExprType getType() override {
      return EXPR_VAR;
    }; 
};

static void generateAdd(std::unique_ptr<Expr>& lhs,
                        std::unique_ptr<Expr>& rhs) {
  printf("add ");
  lhs->generateCode();
  printf(", ");
  rhs->generateCode(); 
}

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
  virtual void generateCode() override {
    switch(op) {
      case BINOP_PLUS:{
      generateAdd(lhs, rhs);
                      }
      default: break;
    }  
  };
  virtual ExprType getType() override {
      return EXPR_BINARY;
  };
};

class ReturnStatement: public Expr {
  std::unique_ptr<Expr> expr;
  LType                 return_type;
  public:
  ReturnStatement(std::unique_ptr<Expr> expr,
                  LType                 type):
                  expr(std::move(expr)), return_type(type) {};
  virtual void generateCode() override {
    switch(return_type) {
      case INT:
        printf("%%return =w ");
        expr->generateCode(); 
        break;
      case VOID:
        printf("ret\n");
        break;
    } 
  };
  virtual ExprType getType() override {
    return EXPR_RETURN;
  };
};

class Declaration: public Expr {
  std::vector<std::unique_ptr<Expr>> stmts;
  ExprType dcl_type;
  public:
  Declaration(std::vector<std::unique_ptr<Expr>> stmts, ExprType dcl_type): stmts(std::move(stmts)), dcl_type(dcl_type) {};
  virtual void generateCode() override {
   switch(dcl_type) {
    case EXPR_VARDEC:
      stmts[0]->generateCode();
      printf(" =w "); // TODO:://  switch depeneding on var type
      stmts[1]->generateCode();
      printf("\n");
      break;
    default: break;
   } 
  };
  virtual ExprType getType() override {
    return dcl_type;
  };
};

class Program: public Expr {
  std::vector<std::unique_ptr<Expr>> declarations;
  public:
  Program(std::vector<std::unique_ptr<Expr>> dcls): declarations(std::move(dcls)) {};  
  virtual void generateCode() override {
    for(auto& dcl: declarations) {
      dcl->generateCode();
      
    } 
  };
  virtual ExprType getType() override {
    return EXPR_PROGRAM;
  };
};
#endif
