#pragma once
#include "Common.h"
#include "Array.h"

typedef enum {
	SCANNER_RUNNING,
	SCANNER_CRASH,
	SCANNER_QUIT,
} ScannerStatus;

typedef struct scanner_t {
	U8* data;
	U32 cursor;
	Array_Type tokens;
	ScannerStatus status;
} ScannerInfo;

typedef enum {
	TOKEN_NONE,
	TOKEN_LEFT_PAREN,
	TOKEN_RIGHT_PAREN,
	TOKEN_LEFT_BRACE,
	TOKEN_RIGHT_BRACE,
	TOKEN_MUL,
	TOKEN_DIV,
	TOKEN_PLUS,
	TOKEN_MINUS,
	TOKEN_LITERAL,
	TOKEN_NUMERIC,
	TOKEN_SEMICOLON,
	TOKEN_COLON,
	TOKEN_COMMA,
	TOKEN_LESS_THAN,
	TOKEN_GREATER_THAN,
	TOKEN_EQUAL,
	TOKEN_AT,

	TOKEN_IDENTIFIER,
	TOKEN_FUNC,
	TOKEN_FOR,
	TOKEN_WHILE,
	TOKEN_IF,
	TOKEN_ELSE,

	TOKEN_DOUBLE_QUOTE,
	TOKEN_SINGLE_QUOTE,

	TOKEN_ILLEGAL,
	TOKEN_ERROR,
	TOKEN_EOF,
	TOKEN_COUNT
} TokenKind;


#ifndef TOKEN_PRINT_TABLE
#define TOKEN_PRINT_TABLE

#endif

typedef struct scannertoken_t {
	TokenKind kind;
	U8* literal;
}* ScannerToken;

ScannerInfo ScannerInit(U8* data, Array_Type tokens);
void ScannerTokenize(U8* data, Array_Type tokens_arr);

void ScannerTokenFree(ScannerToken t);
void ScannerTokenPrint(ScannerToken t);