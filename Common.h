#pragma once

#include "Type.h"
#include "stdarg.h"

#ifndef NULL_DEFINE
#define NULL_DEFINE
#define NULL 0
#endif

void Print(const char* msg, ...);