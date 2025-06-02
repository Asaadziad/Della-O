#include "Scanner.h"
#include "Memory.h"
#include "String.h"



ScannerToken ScannerGetNextToken(U8* data, U32* cursor);

ScannerInfo ScannerInit(U8* data, Array_Type tokens) {	
	return (ScannerInfo) {.data=data, .cursor= 0, .tokens= tokens};
}

void ScannerTokenize(U8* data, Array_Type tokens_arr) {
	if (data == NULL) return;
	
	ScannerInfo sInfo = ScannerInit(data, tokens_arr);

	ScannerToken current_token;
	while (sInfo.status == SCANNER_RUNNING) {
		current_token = ScannerGetNextToken(data, &sInfo.cursor);
		
		
		Array_Push(tokens_arr, current_token);
		
		if (current_token->kind == TOKEN_EOF) {
			sInfo.status = SCANNER_QUIT;
		}
	}

	if (sInfo.status == SCANNER_CRASH) {
		
	}

	Array_Print(tokens_arr);
}



void ScannerTokenFree(ScannerToken t) {
	Free(t->literal);
	Free(t);
}

static const U8* TokenKindPrintTable[TOKEN_COUNT] = {
	[TOKEN_NONE] = {"None"},
	[TOKEN_LEFT_PAREN] = {"LEFT_PAREN"},
	[TOKEN_RIGHT_PAREN] = {"RIGHT_PAREN"},
	[TOKEN_LEFT_BRACE] = {"LEFT_BRACE"},
	[TOKEN_RIGHT_BRACE] = {"RIGHT_BRACE"},
	[TOKEN_MUL] = {"MUL"},
	[TOKEN_DIV] = {"DIV"},
	[TOKEN_PLUS] = {"PLUS"},
	[TOKEN_MINUS] = {"MINUS"},
	[TOKEN_LITERAL] = {"LITERAL"},
	[TOKEN_NUMERIC] = {"NUMERIC"},
	[TOKEN_SEMICOLON] = {"SEMICOLON"},
	[TOKEN_COLON] = {"COLON"},
	[TOKEN_COMMA] = {"COMMA"},
	[TOKEN_LESS_THAN] = {"LESS_THAN"},
	[TOKEN_GREATER_THAN] = {"GREATER_THAN"},
	[TOKEN_EQUAL] = {"EQUAL"},
	[TOKEN_AT] = {"AT"},

	[TOKEN_IDENTIFIER] = {"IDENTIFIER"},
	[TOKEN_FUNC] = {"FUNC"},
	[TOKEN_FOR] = {"FOR"},
	[TOKEN_WHILE] = {"WHILE"},
	[TOKEN_IF] = {"IF"},
	[TOKEN_ELSE] = {"ELSE"},

	[TOKEN_DOUBLE_QUOTE] = {"\""},
	[TOKEN_SINGLE_QUOTE] = {"\'"},

	[TOKEN_ILLEGAL] = {"ILLEGAL"},
	[TOKEN_ERROR] = {"ERROR"},
	[TOKEN_EOF] = {"EOF"},
};

void ScannerTokenPrint(ScannerToken t){
	Print("{ Kind: %s, Value: %s }\n", TokenKindPrintTable[t->kind], t->literal);
}


static ScannerToken TokenCreate(TokenKind kind, U8* literal) {
	ScannerToken token = Malloc(sizeof(*token));
	
	token->kind = kind;
	token->literal = literal;

	return token;
}

static U32 SkipWhiteSpace(U8* data, U32 cursor) {
	U32 tmp = cursor;
	for (;;) {
		char c = data[tmp];
		switch (c) {
		case ' ':
		case '\r':
		case '\t':
		case '\n':
			tmp++;
			break;
		default:
			return tmp;
		}
	}
}

static Bool CharIsAlphabet(char c){
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static Bool CharIsNumeric(char c) {
	return c >= '0' && c <= '9';
}

static U8* ExtractString(const U8* data, U32 cursor, Size_t length) {
	U8* buffer = Malloc(length + 1);
	Memcpy(buffer, data + cursor, length);
	buffer[length] = '\0';
	return buffer;
}

static U8* ExtractNumber(const U8* data, U32* cursor) {
	U32 last_cursor = *cursor;
	while (CharIsNumeric(data[last_cursor])) {
		last_cursor++;
	}
	U32 first_cursor = *cursor;
	*cursor = last_cursor;
	return ExtractString(data, first_cursor, last_cursor - first_cursor);
}

static U8* ExtractLiteral(const U8* data, U32* cursor) {
	U32 last_cursor = *cursor;
	while (CharIsAlphabet(data[last_cursor])) {
		last_cursor++;
	}
	U32 first_cursor = *cursor;
	*cursor = last_cursor;
	return ExtractString(data, first_cursor, last_cursor - first_cursor);
}


static TokenKind GetLiteralKind(U8* literal) {
	if (StringCompare(literal, "func") == 0) {
		return TOKEN_FUNC;
	}

	if (StringCompare(literal, "if") == 0) {
		return TOKEN_IF;
	}

	if (StringCompare(literal, "else") == 0) {
		return TOKEN_ELSE;
	}

	if (StringCompare(literal, "for") == 0) {
		return TOKEN_FOR;
	}

	if (StringCompare(literal, "while") == 0) {
		return TOKEN_WHILE;
	}

	return TOKEN_IDENTIFIER;
}

static ScannerToken ScannerGetNextToken(U8* data, U32* cursor) {
	U32 next_token_length = 0;

	U32 next_cursor = SkipWhiteSpace(data, *cursor);
	*cursor = next_cursor;
	char current_char = data[next_cursor];
	
	if (current_char == '\0') return TokenCreate(TOKEN_EOF, NULL);
	
	TokenKind current_kind = TOKEN_NONE;
	switch (current_char) {
		case '(':
			current_kind = TOKEN_LEFT_PAREN;
			break;
		case ')':
			current_kind = TOKEN_RIGHT_PAREN;
			break;
		case '{':
			current_kind = TOKEN_LEFT_BRACE;
			break;
		case '}':
			current_kind = TOKEN_RIGHT_BRACE;
			break;
		case '*':
			current_kind = TOKEN_MUL;
			break;
		case '/':
			current_kind = TOKEN_DIV;
			break;
		case '+':
			current_kind = TOKEN_PLUS;
			break;
		case '-':
			current_kind = TOKEN_MINUS;
			break;
		case ';':
			current_kind = TOKEN_SEMICOLON;
			break;
		case ':':
			current_kind = TOKEN_COLON;
			break;
		case ',':
			current_kind = TOKEN_COMMA;
			break;
		case '<':
			current_kind = TOKEN_LESS_THAN;
			break;
		case '>':
			current_kind = TOKEN_GREATER_THAN;
			break;
		case '=':
			current_kind = TOKEN_EQUAL;
			break;
		case '@':
			current_kind = TOKEN_AT;
			break;
		case '\"':
			current_kind = TOKEN_DOUBLE_QUOTE;
			break;
		case '\'':
			current_kind = TOKEN_SINGLE_QUOTE;
			break;
	}

	if (current_kind != TOKEN_NONE) {
		*cursor = *cursor + 1;
		
		U8* allocated_char = Malloc(sizeof(char) * 2);
		allocated_char[0] = current_char;
		allocated_char[1] = '\0';

		return TokenCreate(current_kind, allocated_char);
	}


	if (CharIsAlphabet(current_char)) {
		U8* literal = ExtractLiteral(data, cursor);		
		TokenKind literal_kind = GetLiteralKind(literal);
		
		return TokenCreate(literal_kind, literal);
	}

	if (CharIsNumeric(current_char)) {
		U8* literal = ExtractNumber(data, cursor);
		return TokenCreate(TOKEN_NUMERIC, literal);
	}



	*cursor = *cursor + 1;
	return TokenCreate(TOKEN_ILLEGAL, NULL);
}

