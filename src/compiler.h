#ifndef COMPILER_H
#define COMPILER_H

#include "expressions.h"
#include <cstdio>
#include <string>

void gen_primary(std::unique_ptr<Expr> exp, FILE* out, int* stack_size);


#endif
