#include <string>
#include <memory>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include "expressions.h"


extern std::map<std::string, bool> globals;
extern std::map<std::string, LType> globals_types;

  static void gen_binop(BinopType op, FILE* out) {
    switch(op) { 
      case BINOP_PLUS:
        fprintf(out, "add"); 
      break;
      case BINOP_MULT:
        fprintf(out, "mul");
        break;
      case BINOP_MOD:
        fprintf(out, "rem");
        break; 
      default: break;
    };
  }
  static void gen_number(FILE* out, double val) {
    fprintf(out, "%d", (int)val);
  }

  static void gen_var_literal(FILE* out, char literal) {
    fprintf(out, "%%%c", literal);
  }
  
  static void gen_type(FILE* out, LType type) {
    switch(type){
      case INT:
        fprintf(out, "w");
        break;
      case STRING:
        fprintf(out, "l");
        break;
      case BOOL: // bool would be a byte as global and word as local
        fprintf(out, "b");
        break;
      default : break;
    }
  }
  

  static void gen_assign(FILE* out, const char* ptr, LType type) {
    fprintf(out, "%s =", ptr);
    gen_type(out, type);
  }
  
  // copies the value stored in ptr2 into ptr1
  static void gen_copy(FILE* out, LType type, const char* ptr1, const char* ptr2) {
    gen_assign(out, ptr1, type);
    fprintf (out, "copy %s", ptr2);
  }
  
  // stores a byte in pointer
  static void gen_storeb(FILE* out, char byte, const char* ptr) {
    fprintf(out, "storeb %d, %s\n", byte, ptr);
  }
  static void gen_storew() {}
  static void gen_storel() {} 

  void StringExpr::generateCode(FILE* out, int* stack_size)  {
    size_t len = val.size();
    fprintf(out, "%%A%d =l alloc4 %lu\n", *stack_size ,len);
    fprintf(out, "%%current_p =l copy %%A%d\n", *stack_size); 
    for(int i= 0 ; i < len; i++) {
        gen_storeb(out, val.c_str()[i], "%current_p");// *current_p = char;
        fprintf(out, "%%current_p =l add %%current_p, 1\n"); //  current_p++;
    }  
    fprintf(out, "%%s%d =l copy %%A%d\n", *stack_size, *stack_size);
    *stack_size += 1;
  };
  
  ExprType StringExpr::getType() {
    return EXPR_STRING;
  };
  LType StringExpr::getDataType() {
    return STRING;
  };

  ExprType NumberExpr::getType()  {
   return EXPR_NUMBER;
  }
    void NumberExpr::generateCode(FILE* out, int* stack_size) {
       gen_number(out, val); 
       //fprintf(out, "%%s%d =w copy %d\n", *stack_size,(int)val);
       *stack_size += 1;
    };
    LType NumberExpr::getDataType()  {
      return INT;
    };

    ExprType BoolExpr::getType() {
      return EXPR_BOOL;
    };
    void BoolExpr::generateCode(FILE* out, int* stack_size) {
        fprintf(out, "%%s%d =w copy %d\n", *stack_size,(bool)val);
        *stack_size += 1;
    }
    LType BoolExpr::getDataType(){
      return BOOL;
    }

void VarExpr::setVarType(LType type) { var_type = type; };


   ExprType VarExpr::getType() {
      return EXPR_VAR;
    };
   LType VarExpr::getDataType() {
    return var_type;
   }; 
   std::string VarExpr::getName() {
    return id_name;
   };
   void VarExpr::generateCode(FILE* out, int* stack_size)  {      
    fprintf(out,"%%%s", id_name.c_str());      
  };
  LType VarExpr::getReturnType() {
    return var_type;
  };

ExprType FunCall::getType() {
      return EXPR_FUNCALL;
    };
   LType FunCall::getDataType() {
    return return_type;
   }
   void FunCall::generateCode(FILE* out, int* stack_size)  {
    if(!globals[name]){ 
      printf("Compile error: undeclared function '%s'", name.c_str()); 
      return;
    }

    std::vector<int> fixed_args;
    for(auto& arg: args) {
      if(arg->getType() == EXPR_VAR) {  
        fixed_args.push_back(-1);
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
      if(i > 0) {
       fprintf(out , ",");
      }
      if(fixed_args[i] == -1) {
        VarExpr* tmpv = static_cast<VarExpr*>(args[i].release());
        LType type = (tmpv->getDataType());
        switch(type) { 
          case STRING:
            fprintf(out, "l ");
            break;
          default : fprintf(out, "w ");
        }
        tmpv->generateCode(out, stack_size);
        continue;
      }
      fprintf(out, "w %%s%d", *stack_size - 1); 
    } 
    fprintf(out, ")\n");
  };
  
  ExprType ComparisionExpr::getType() {
      return EXPR_COMPARISION;
    };
   LType ComparisionExpr::getDataType() {
      return BOOL;
    };
    void ComparisionExpr::generateCode(FILE* out, int* stack_size) {
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

  ExprType BinaryExpr::getType() {
      return EXPR_BINARY;
  };
  LType BinaryExpr::getDataType() {
    return INT;
  };
  BinopType BinaryExpr::getOpType() {
    return op;
  }
  std::unique_ptr<Expr> BinaryExpr::getLhs() {
    return std::move(lhs);
  }
  std::unique_ptr<Expr> BinaryExpr::getRhs() {
    return std::move(rhs);
  }
  void BinaryExpr::generateCode(FILE* out, int* stack_size)  {
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
      case BINOP_MOD:
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
        fprintf(out, "%%s%d =w rem %%s%d, %%s%d\n", *stack_size - 2, *stack_size - 2, *stack_size -1); 
        *stack_size -= 1;
        break;
      case BINOP_ASSIGN:
        {
        BinaryExpr* rhs_p = static_cast<BinaryExpr*>(rhs.release());
        VarExpr* lhs_p = static_cast<VarExpr*>(lhs.release());     
        if(rhs_p->getDataType() !=
          globals_types[lhs_p->getName()]){
           std::cerr << "Cant assign value of type " << rhs_p->getDataType() << "to variable of type" << 
             globals_types[lhs_p->getName()] << std::endl; 
           break;
        }
        rhs_p->generateCode(out, stack_size);
        lhs_p->generateCode(out,stack_size);
        switch(rhs_p->getDataType()){
          case INT:
            fprintf(out," =w");
            break;
          case STRING:
            fprintf(out, " =l");
            break;
          case BOOL:
            fprintf(out, " =w");
            break;
          default:
            fprintf(out, " =w");
            break;
        }
        
        fprintf(out, " copy %%s%d\n", *stack_size - 1);
    }
        break;
      default: break; 
    }
  };

  ExprType ReturnStatement::getType() {
    return EXPR_RETURN;
  };
  void ReturnStatement::generateCode(FILE* out, int* stack_size)  {  
      expr->generateCode(out, stack_size);
      fprintf(out, "\n%%r =w copy %%s%d\n", *stack_size - 1);
      fprintf(out, "jmp @retstmt\n");
  };

  ExprType PrintStatement::getType() {
    return EXPR_FUNCALL;
  };
  void PrintStatement::generateCode(FILE* out, int* stack_size)  {
    if(string_expr->getType() == EXPR_VAR) { 
      auto tmp  = string_expr.release(); 
      VarExpr* v = static_cast<VarExpr*> (tmp); 
      char type_c = ' ';
      switch(v->getDataType()) { 
        case STRING:
          {
           type_c = 'l';
           fprintf(out, "call $printf(%c %%%s)\n",type_c, v->getName().c_str());
          } 
         break;
        default:
         {
           type_c = 'w';
           fprintf(out, "call $printf(l $fmt_int, %c %%%s)\n",type_c, v->getName().c_str());
          }  
         break;
      } 
    } else {
    string_expr->generateCode(out, stack_size);
    fprintf(out, "call $printf(l %%s%d)\n", *stack_size - 1);
    }
  };

void ForStatement::generateCode(FILE* out, int* stack_size) {
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
  ExprType ForStatement::getType() {
    return EXPR_FOR;
  }

  ExprType VarDeclaration::getType(){
    return EXPR_VARDEC;
  };
  void VarDeclaration::generateCode(FILE* out, int* stack_size)  { 
    var_expr->generateCode(out, stack_size); 
    VarExpr* var_name_p = static_cast<VarExpr*>(var_name.release());
    var_name_p->generateCode(out, stack_size);
    switch(globals_types[var_name_p->getName()]) {
      case INT:
        fprintf(out, " =w");
        break;
      case STRING:
        fprintf(out, " =l");
        break;
      case BOOL:
        fprintf(out, " =b");
        break;
      default:break;
    }
    
    fprintf(out, " copy %%s%d\n", *stack_size - 1);
  }; 
  std::string VarDeclaration::getName() {
    return name_str;
  }

  BlockType Block::getBlockType() {
    return type;
  }
  void Block::setBlockType(BlockType type_in) {
    type = type_in;
  }
  ExprType Block::getType() {
    return EXPR_BLOCK;
  };
  void Block::generateCode(FILE* out, int* stack_size) {
    for(auto& decl : decls) {
      if(decl->getType() == EXPR_VARDEC) {
        VarDeclaration* tmp = static_cast<VarDeclaration*>(decl.release());
        if(!local_variables[tmp->getName()]) { 
        local_variables[tmp->getName()] = true; 
        tmp->generateCode(out, stack_size);
        } else {
          std::cout << "Compile time error: already declared " << tmp->getName() << std::endl; 
        }
        continue;
      }
      
      decl->generateCode(out, stack_size); 
    }
  }

void IfStatement::generateCode(FILE* out, int* stack_size) {
    
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
    Block* tmpb = static_cast<Block*>(tBlock.release());
    tmpb->generateCode(out, stack_size);
    if(tmpb->getBlockType() == VOID_BLOCK) { 
      fprintf(out, "jmp @ifend%d\n", stmt_id);
    }
    fprintf(out, "@iff%d\n", stmt_id);
    if(fBlock) {
      fBlock->generateCode(out, stack_size); 
      fprintf(out, "\n");
    }
    fprintf(out, "@ifend%d\n", stmt_id);
  };
  ExprType IfStatement::getType() {
    return EXPR_FOR;
  }


  ExprType FunDeclaration::getType()  {
    return EXPR_FUNDEC;
  };
  void FunDeclaration::generateCode(FILE* out, int* stack_size) {
      char type_c = ' ';
      switch(type) {
        case BOOL:
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
        
      Block* tmp = static_cast<Block*>(block.release()); 
      tmp->generateCode(out, stack_size);
      fprintf(out, "\n@retstmt\n");
      if(tmp->getBlockType() == VOID_BLOCK) { 
        fprintf(out, "\nret\n");
      } else {
        fprintf(out, "\nret %%r");
      }
      fprintf(out, "\n}\n"); 
  }


  ExprType Program::getType() {
    return EXPR_PROGRAM;
  };
  void Program::generateCode(FILE* out, int* stack_size)  {
    for(auto& dcl : dcls) {
      dcl->generateCode(out, stack_size);
    }
  };
