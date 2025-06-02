#include "Compiler.h"
#include "Scanner.h"
#include "Memory.h"
#include "FS.h"



void CompilerMain(const char* file_path) {
	U8* data = FS_ReadFile(file_path);
	CompilerInfo compiler_info;
	ScannerToken t;
	compiler_info.tokens = Array_Create(10, sizeof(*t));

	Array_SetPrintFn(compiler_info.tokens, ScannerTokenPrint);
	Array_SetFreeFn(compiler_info.tokens, ScannerTokenFree);

	ScannerTokenize(data, compiler_info.tokens);
	//CreateParseTree(data);
	//CreateAnalysis(data);
	//GenerateIR(data);

	Print("%s", data);
}