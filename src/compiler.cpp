#include "compiler.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>


void compileLetDeclaration(std::unique_ptr<Expr> root, std::string var_name){
  FILE* out = fopen("main.ssa", "w+");
  if(!out) {
    std::cerr << "Could'nt open file" << std::endl;
    return;
  } 
  int stack_size = 0;
  root->generateCode(out, &stack_size);
  fprintf(out, "   %%%s =w copy %%s%d", var_name.c_str(), stack_size - 1);

  fclose(out);
}

void compile(std::unique_ptr<Expr> root){
  FILE* out = fopen("main.ssa", "w+");
  if(!out) {
    std::cerr << "Could'nt open file" << std::endl;
    return;
  } 
  int stack_size = 0;
  root->generateCode(out,&stack_size );
  fclose(out);  
}

