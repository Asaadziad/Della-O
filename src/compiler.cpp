#include "compiler.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>

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

