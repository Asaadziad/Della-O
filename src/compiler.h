#ifndef COMPILER_H
#define COMPILER_H

#include <memory>
#include "expressions.h"

void compile(std::unique_ptr<Expr> root);

#endif
