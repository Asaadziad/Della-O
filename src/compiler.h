#ifndef COMPILER_H
#define COMPILER_H

#include "expressions.h"
#include <cstdio>
#include <string>


void compile(std::unique_ptr<Expr> exp);

#endif
