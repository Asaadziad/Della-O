#ifndef COMPILER_H
#define COMPILER_H

#include <memory>
#include <string>
#include "expressions.h"


void compileLetDeclaration(std::unique_ptr<Expr> root, std::string var_name);
void compile(std::unique_ptr<Expr> root);

#endif
