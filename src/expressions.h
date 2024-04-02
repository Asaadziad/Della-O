#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include <string>
#include <memory>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <unordered_map>


typedef std::unordered_map<std::string, bool> Vars; 

typedef struct locals {
  std::vector<Vars>   variables;
  size_t current_scope;
  size_t scope_depth;
} Locals;

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
  BINOP_MOD,    
  BINOP_ASSIGN,
} BinopType;


typedef enum {
  EXPR_NUMBER,
  EXPR_BOOL,
  EXPR_VAR,
  EXPR_STRING,
  
  EXPR_BINARY,
  EXPR_COMPARISION, 
  
  EXPR_VARDEC,
  EXPR_DEC,
  EXPR_FUNDEC,
  

  EXPR_FOR,
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
  virtual void generateCode(FILE* out, int* stack_size, Locals variables_storage) = 0; 
};

class DataExpr {
  public:
  virtual LType    getDataType() = 0;
};

class NamedExpr {
  public:
  virtual std::string getName() = 0;
};

class StringExpr: public Expr, public DataExpr {
  std::string val;
  public:
  StringExpr(std::string val): val(std::move(val)) {};
  virtual void generateCode(FILE* out, int* stack_size, Locals variables_storage) override;
  virtual ExprType getType() override;
  virtual LType getDataType() override;
};

class NumberExpr: public Expr, public DataExpr { 
  double val;
  public:
    NumberExpr(double val): val(val) {};  
    virtual ExprType getType() override;
    virtual void generateCode(FILE* out, int* stack_size, Locals variables_storage) override;
    virtual LType getDataType() override;
};

class BoolExpr: public Expr, public DataExpr {
  bool val;
  public:
    BoolExpr(bool val): val(val) {};
    virtual ExprType getType() override;
    virtual void generateCode(FILE* out, int* stack_size, Locals varia
        ) override; 
    virtual LType getDataType() override; 
};


class VarExpr: public Expr, public DataExpr, public NamedExpr {
  std::string id_name;
  LType       var_type;
  public:
  VarExpr(std::string id_name):id_name(id_name), var_type(VOID) {};
  VarExpr(std::string id_name, LType type): id_name(id_name), var_type(type){};
  void setVarType(LType type);   
   virtual ExprType getType() override;
   virtual LType getDataType() override; 
   virtual std::string getName() override ;
  virtual void generateCode(FILE* out, int* stack_size, Locals variables) override ;
  LType getReturnType();
};

class FunCall: public Expr, public DataExpr {
  std::string name;
  std::vector<std::unique_ptr<Expr>> args;
  LType return_type;
  public:
  FunCall(std::string name, std::vector<std::unique_ptr<Expr>> args): name(name), args(std::move(args)){};
   virtual ExprType getType() override;
   virtual LType getDataType() override; 
  virtual void generateCode(FILE* out, int* stack_size, Locals varia
      ) override;

};

typedef enum {
  COMPARE_GT,
  COMPARE_GT_EQ,
  COMPARE_LT,
  COMPARE_LT_EQ, 
  COMPARE_EQ_EQ,
} ComparisionType;

class ComparisionExpr: public Expr, public DataExpr {
  std::unique_ptr<Expr> lhs;
  std::unique_ptr<Expr> rhs;
  ComparisionType       op;
  public:
    ComparisionExpr(std::unique_ptr<Expr> lhs,
                    std::unique_ptr<Expr> rhs,
                    ComparisionType op):
                    lhs(std::move(lhs)),
                    rhs(std::move(rhs)),
                    op(op){};
    virtual ExprType getType() override;
    virtual LType getDataType() override;
    virtual void generateCode(FILE* out, int* stack_size, Locals varia
        ) override;
};

class BinaryExpr : public Expr, public DataExpr {
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
  
  virtual ExprType getType() override;
  virtual LType getDataType() override ;
  BinopType getOpType(); 
  std::unique_ptr<Expr> getLhs(); 
  std::unique_ptr<Expr> getRhs(); 
  virtual void generateCode(FILE* out, int* stack_size, Locals varia
      ) override;
};

class ReturnStatement: public Expr {
  std::unique_ptr<Expr> expr;
  LType                 return_type;
  public:
  ReturnStatement(std::unique_ptr<Expr> expr,
                  LType                 type):
                  expr(std::move(expr)), return_type(type) {};
  
  virtual ExprType getType() override ;
  virtual void generateCode(FILE* out, int* stack_size, Locals varia
      ) override ;
  
};

class PrintStatement: public Expr {
  std::unique_ptr<Expr> string_expr;
  public:
  PrintStatement(std::unique_ptr<Expr> expr): string_expr(std::move(expr)) {};
  virtual ExprType getType() override ;
  virtual void generateCode(FILE* out, int* stack_size, Locals variables_storage) override; 
};

class ForStatement: public Expr {
  std::unique_ptr<Expr> left_exp;
  std::unique_ptr<Expr> right_exp;
  std::unique_ptr<Expr> block;
  std::string range;
  public:
  ForStatement(std::unique_ptr<Expr> block, std::string range, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right):left_exp(std::move(left)), right_exp(std::move(right)), block(std::move(block)), range(std::move(range)){};
  virtual void generateCode(FILE* out, int* stack_size, Locals varia
      ) override ;
  virtual ExprType getType() override; 
};

typedef enum {
  RETURNED_BLOCK,
  VOID_BLOCK,
} BlockType;


class Block : public Expr {  
  std::vector<std::unique_ptr<Expr>> decls;  
  BlockType type;
  public:
  Block(std::vector<std::unique_ptr<Expr>> decls
        ): decls(std::move(decls)), type(VOID_BLOCK){
  
  };
  BlockType getBlockType(); 
  void setBlockType(BlockType type_in);
  virtual ExprType getType() override ;
  virtual void generateCode(FILE* out, int* stack_size, Locals varia
      ) override; 
};


class IfStatement: public Expr {
  std::unique_ptr<Expr> condition;
  std::unique_ptr<Expr> tBlock;
  std::unique_ptr<Expr> fBlock;
  public:
  IfStatement(std::unique_ptr<Expr> condition,
              std::unique_ptr<Expr> tBlock,
              std::unique_ptr<Expr> fBlock):
              condition(std::move(condition)),
              tBlock(std::move(tBlock)),
              fBlock(std::move(fBlock)){}
  
  virtual void generateCode(FILE* out, int* stack_size, Locals varia
      ) override ;
  virtual ExprType getType() override; 
  
};


class VarDeclaration: public Expr {
  std::unique_ptr<Expr>   var;
  std::unique_ptr<Expr>   var_expr;
  public:
    VarDeclaration(std::unique_ptr<Expr> var,
                  std::unique_ptr<Expr> var_expr): var(std::move(var)), var_expr(std::move(var_expr)) {};
  virtual ExprType getType() override ;
  virtual void generateCode(FILE* out, int* stack_size, Locals varia
      ) override ;
};
//  function name : string
//  arguments : var expression
//  function body : block 
class FunDeclaration: public Expr {
  std::string name;
  std::vector<std::unique_ptr<VarExpr>> args; 
  std::unique_ptr<Expr>                 block;  
  LType                                 type; 
  public:
  FunDeclaration(std::string name,
                std::vector<std::unique_ptr<VarExpr>> a, 
                std::unique_ptr<Expr> block, 
                LType type):
                name(std::move(name)),
                args(std::move(a)), 
                block(std::move(block)),
               
                type(type) {};
  
  virtual ExprType getType() override ;
  virtual void generateCode(FILE* out, int* stack_size, Locals varia
      ) override ;
};

class Program: public Expr {
  std::vector<std::unique_ptr<Expr>> dcls; 
  public:
  Program(std::vector<std::unique_ptr<Expr>> exprs): dcls(std::move(exprs)){};
  virtual ExprType getType() override ;
  virtual void generateCode(FILE* out, int* stack_size, Locals varia
      ) override;
};
#endif
