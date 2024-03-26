#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include <string>
#include <memory>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>

extern std::map<std::string, bool>  globals;

typedef enum {
  VOID,
  INT,
  STRING,
  BOOL,
} LType;

extern std::map<std::string, LType> globals_types;
typedef enum {
  BINOP_PLUS,
  BINOP_MINUS,
  BINOP_MULT,
  BINOP_DIVIDE,
    
  BINOP_ASSIGN,
} BinopType;

typedef enum {
  EXPR_NUMBER,
  EXPR_BOOL,
  EXPR_VAR,
  EXPR_BINARY,
  EXPR_COMPARISION, 
  
  EXPR_VARDEC,
  EXPR_DEC,
  EXPR_FUNDEC,
  EXPR_STRING,

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
  virtual void generateCode(FILE* out, int* stack_size) = 0; 
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
  };
  virtual LType getDataType() override {
    return STRING;
  };
};

class NumberExpr: public Expr, public DataExpr { 
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
    virtual LType getDataType() override {
      return INT;
    };
};

class BoolExpr: public Expr, public DataExpr {
  bool val;
  public:
    BoolExpr(bool val): val(val) {};
    virtual ExprType getType() override {
      return EXPR_BOOL;
    };
    virtual void generateCode(FILE* out, int* stack_size) override {}
    virtual LType getDataType() override {
      return BOOL;
    }
};


class VarExpr: public Expr, public DataExpr, public NamedExpr {
  std::string id_name;
  LType       var_type;
  public:
  VarExpr(std::string id_name):id_name(id_name), var_type(VOID) {};
  VarExpr(std::string id_name, LType type): id_name(id_name), var_type(type){};
  void setVarType(LType type) { var_type = type; };   
   virtual ExprType getType() override {
      return EXPR_VAR;
    };
   virtual LType getDataType() override {
    return var_type;
   }; 
   virtual std::string getName() override {
    return id_name;
   };
  virtual void generateCode(FILE* out, int* stack_size) override {      
    fprintf(out,"%%%s", id_name.c_str());      
  };
  LType getReturnType() {
    return var_type;
  };
};

class FunCall: public Expr, public DataExpr {
  std::string name;
  std::vector<std::unique_ptr<Expr>> args;
  LType return_type;
  public:
  FunCall(std::string name, std::vector<std::unique_ptr<Expr>> args): name(name), args(std::move(args)){};
   virtual ExprType getType() override {
      return EXPR_FUNCALL;
    };
   virtual LType getDataType() override {
    return return_type;
   }
  virtual void generateCode(FILE* out, int* stack_size) override {
    if(!globals[name]){ 
      printf("Compile error: undeclared function '%s'", name.c_str()); 
      return;
    }

    std::vector<int> fixed_args;
    for(auto& arg: args) {
      if(arg->getType() == EXPR_VAR) {
        continue;
      };
      arg->generateCode(out, stack_size);
      fixed_args.push_back(*stack_size - 1);
    }
    if(globals_types[name] != VOID) {
      char type_c = 'w';
      fprintf(out, "%%s%d =%c ", *stack_size - 1 ,type_c);
    }  
    fprintf(out, "call $%s(", name.c_str());
    for(int i = 0; i < fixed_args.size(); i++) {
      if(i>0) {
        fprintf(out, ", ");
      }
      fprintf(out, "w %%s%d", fixed_args[i]);
    }
    fprintf(out, ")\n");
  };

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
    virtual ExprType getType() override {
      return EXPR_COMPARISION;
    };
    virtual LType getDataType() override {
      return BOOL;
    };
    virtual void generateCode(FILE* out, int* stack_size) override {
      int c_id = *stack_size;
      if(lhs->getType() == EXPR_VAR) {
         fprintf(out, "%%le%d =w copy ", c_id);
         lhs->generateCode(out, stack_size);
         fprintf(out, "\n");
      } else {
        lhs->generateCode(out, stack_size);
        fprintf(out, "%%le%d =w copy %%s%d\n", c_id, *stack_size - 1);
      } 
      if(rhs->getType() == EXPR_VAR) {
         fprintf(out, "%%re%d =w copy ", c_id);
         rhs->generateCode(out, stack_size);
         fprintf(out, "\n");
      } else {
        rhs->generateCode(out, stack_size);
        fprintf(out, "%%re%d =w copy %%s%d\n", c_id, *stack_size - 1);
      }
      *stack_size += 1;// hacky TODO:: change
      switch(op) {
        case COMPARE_EQ_EQ:
           
          fprintf(out, "%%s%d =w ceqw %%le%d, %%re%d\n",*stack_size - 1, c_id, c_id);
          break; 
        case COMPARE_GT:
          fprintf(out, "%%s%d =w csgtw %%le%d, %%re%d\n",*stack_size - 1, c_id, c_id);
          break;
        case COMPARE_GT_EQ:
         fprintf(out, "%%s%d =w csgew %%le%d, %%re%d\n",*stack_size - 1, c_id, c_id);
          break; 
        case COMPARE_LT:
        fprintf(out, "%%s%d =w csltw %%le%d, %%re%d\n",*stack_size - 1, c_id, c_id);  
          break;
        case COMPARE_LT_EQ:
          fprintf(out, "%%s%d =w cslew %%le%d, %%re%d\n",*stack_size - 1, c_id, c_id);
          break;
        default:break;
      }
    
    };
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
  
  virtual ExprType getType() override {
      return EXPR_BINARY;
  };
  virtual LType getDataType() override {
    return INT;
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
  virtual void generateCode(FILE* out, int* stack_size) override {
    expr->generateCode(out, stack_size);
    fprintf(out, "\nret %%s%d\n", *stack_size - 1);
  };
  
};

class PrintStatement: public Expr {
  std::unique_ptr<Expr> string_expr;
  public:
  PrintStatement(std::unique_ptr<Expr> expr): string_expr(std::move(expr)) {};
  virtual ExprType getType() override {
    return EXPR_FUNCALL;
  };
  virtual void generateCode(FILE* out, int* stack_size) override {
    if(string_expr->getType() == EXPR_VAR) { 
      auto tmp  = string_expr.release(); 
      VarExpr* v = static_cast<VarExpr*> (tmp); 
      char type_c = ' ';
      switch(v->getDataType()) { 
        case STRING:
          {
           type_c = 'l';
           fprintf(out, "call $printf(%c %%%s)",type_c, v->getName().c_str());
          } 
         break;
        default:
         {
           type_c = 'w';
           fprintf(out, "call $printf(l $fmt_int, %c %%%s)",type_c, v->getName().c_str());
          }  
         break;
      } 
    } else {
    string_expr->generateCode(out, stack_size);
    fprintf(out, "call $printf(l %%s%d)\n", *stack_size - 1);
    }
  }; 
};

static std::vector<int> splitRange(std::string range) {
  std::vector<int> rangee;
  std::string tmp = "";
  int cnt = 0;
  for(auto& ch : range) {
     if(ch == '.') { 
       cnt++;
       if(cnt > 1) {
         rangee.push_back(std::stoi(tmp.c_str()));
       }
       continue;
     }
     tmp += ch;
  }
 rangee.push_back(std::stoi(tmp.c_str()));
  
  return rangee; 
}

class ForStatement: public Expr {
  std::unique_ptr<Expr> left_exp;
  std::unique_ptr<Expr> right_exp;
  std::unique_ptr<Expr> block;
  std::string range;
  public:
  ForStatement(std::unique_ptr<Expr> block, std::string range, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right):left_exp(std::move(left)), right_exp(std::move(right)), block(std::move(block)), range(std::move(range)){};
  virtual void generateCode(FILE* out, int* stack_size) override {
/*     for 0..100 translates to: 
 *      %x =w copy 100
        %s =w copy 0
@loop
        %s =w add %s, %x
        %x =w sub %x, 1
        jnz %x, @loop, @end
@end
*/  if(!left_exp && !right_exp) {
      fprintf(out, "%%lr =w copy 0\n");
      fprintf(out, "@loop\n");
      fprintf(out, "%%lr =w add %%lr, 1\n");
      block->generateCode(out, stack_size);
      fprintf(out, "jnz %%lr, @loop, @end\n");
      fprintf(out, "@end\n");
      return;
    }

      
      
      if(left_exp->getType() == EXPR_VAR) {
        fprintf(out, "%%start =w copy ");
        left_exp->generateCode(out, stack_size);
        fprintf(out, "\n");
        
      } else {
        left_exp->generateCode(out, stack_size);
        fprintf(out, "%%start =w copy %%s%d\n", *stack_size - 1);
      } 
      if(right_exp->getType() == EXPR_VAR) {
        fprintf(out, "%%end =w copy ");
        right_exp->generateCode(out, stack_size);
        fprintf(out, "\n");
        
      } else {
        right_exp->generateCode(out, stack_size);
        fprintf(out, "%%end =w copy %%s%d\n", *stack_size - 1);      
      }
      
      fprintf(out, "%%lr =w sub %%end, %%start\n");
      fprintf(out, "@loop\n");
      fprintf(out, "%%lr =w sub %%lr, 1\n");
      block->generateCode(out, stack_size);
      fprintf(out, "jnz %%lr, @loop, @end\n");
      fprintf(out, "@end\n"); 
    
  };
  virtual ExprType getType() override {
    return EXPR_FOR;
  }
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
  
  virtual void generateCode(FILE* out, int* stack_size) override {
    
    int stmt_id = *stack_size; 
    
    if(condition->getType() == EXPR_VAR) {
      fprintf(out, "%%condition%d =w copy", stmt_id);
      condition->generateCode(out, stack_size); 
      fprintf(out, "\n");
    } else {
      condition->generateCode(out, stack_size);
      fprintf(out, "%%condition%d =w copy %%s%d\n", stmt_id, *stack_size - 1);
    }
        fprintf(out, "@ifstmt\n");
    fprintf(out, "jnz %%condition%d, @ift%d, @iff%d\n", stmt_id, stmt_id, stmt_id);
    fprintf(out, "@ift%d\n", stmt_id);
    *stack_size += 1; // hacky solution for a bug TODO:: find real solution
    tBlock->generateCode(out, stack_size);
    fprintf(out, "jmp @ifend%d\n", stmt_id);
    fprintf(out, "@iff%d\n", stmt_id);
    if(fBlock) {
      fBlock->generateCode(out, stack_size); 
      fprintf(out, "\n");
    }
    
    fprintf(out, "@ifend%d\n", stmt_id); 
    


  };
  virtual ExprType getType() override {
    return EXPR_FOR;
  }
  
};


class Block : public Expr {
  std::map<std::unique_ptr<Expr> ,bool> locals;
  std::vector<std::unique_ptr<Expr>> decls;  
  public:
  Block(std::vector<std::unique_ptr<Expr>> decls
        ): decls(std::move(decls)){};
  
  virtual ExprType getType() override {
    return EXPR_BLOCK;
  };
  virtual void generateCode(FILE* out, int* stack_size) override {
    for(auto& decl : decls) {
      if(decl->getType() == EXPR_VAR) {
        
      }
      decl->generateCode(out, stack_size); 
    }
  }; 
};



//  function name : string
//  arguments : var expression
//  function body : block 
class FunDeclaration: public Expr {
  std::string name;
  std::vector<std::unique_ptr<VarExpr>> args; 
  std::unique_ptr<Expr>              block; 
  std::unique_ptr<ReturnStatement> return_stmt;
  LType                              type;
  public:
  FunDeclaration(std::string name,
                std::vector<std::unique_ptr<VarExpr>> a, 
                std::unique_ptr<Expr> block,
                std::unique_ptr<ReturnStatement> return_stmt,
                LType type):
                name(std::move(name)),
                args(std::move(a)), 
                block(std::move(block)),
                return_stmt(std::move(return_stmt)),
                type(type) {};
  
  virtual ExprType getType() override {
    return EXPR_FUNDEC;
  };
  virtual void generateCode(FILE* out, int* stack_size) override {
      char type_c = ' ';
      switch(type) {
        case INT:
          type_c = 'w';
          break;
        case STRING:
          type_c = 'l';
          break;
        default: break;
      }
      if(name.compare("main") == 0) {
        fprintf(out, "export function %c $%s(", type_c ,name.c_str());
      } else {
        fprintf(out, "function %c $%s(", type_c ,name.c_str());
      }
      int i = 0;
      for(auto& arg: args) {
        if(i > 0) {
          fprintf(out, ", ");
        }
        char type_char = ' ';
        switch(arg->getReturnType()) {
          case INT:
          type_char = 'w';
          break;
          case STRING:
          type_char = 'l';
          break;
          default : break;
        };
        fprintf(out, "%c ", type_char);      
        arg->generateCode(out, stack_size);
        i++;
      }
      fprintf(out, "){\n@start\n");
      block->generateCode(out, stack_size); 
      if(type != VOID) {
        return_stmt->generateCode(out, stack_size);      
      } else {
        fprintf(out,"\nret\n");
      }
      fprintf(out, "\n}\n"); 
  };

};

class VarDeclaration: public Expr {
  std::unique_ptr<Expr>   var_name;
  std::unique_ptr<Expr>      var_expr;
  ExprType dcl_type;
  public:
  VarDeclaration(std::unique_ptr<Expr> name, std::unique_ptr<Expr> var_expr): var_name(std::move(name)), var_expr(std::move(var_expr)){}; 
  virtual ExprType getType() override {
    return EXPR_VARDEC;
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
      dcl->generateCode(out, stack_size);
    }
  };
};
#endif
