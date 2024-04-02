#include "compiler.h"

void Compiler::compile() {
  FILE* out = fopen("main.ssa", "w+");
  if(!out) {
    std::cout << "hhelp";
    return;
  }
  int stack_size = 0;
  root->generateCode(out, &stack_size, 
                      );
  fclose(out);
}
