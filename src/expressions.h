#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include <string>
#include <memory>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <iostream>

typedef enum {
  BINOP_PLUS,
  BINOP_MULT,
  BINOP_ASSIGN,
} BinopType;

typedef enum {
  EXPR_NUMBER,
  EXPR_VAR,
} ExprType;



#endif
