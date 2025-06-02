#include "Logger.h"
#include "Compiler.h"


int main(int argc, char** argv) {
	argc = 2;
	argv[1] = "C:/Users/win10/workspace/Della-O/main.della";
	if (argc < 2) {
		LOG_ERROR("Expected file path");
		return 1;
	}

	if (argc > 3) {
		LOG_ERROR("More than 1 file is not currently supported");
		return 1;
	}

	char* file_path = argv[1];
	CompilerMain(file_path);


	return 0;
}