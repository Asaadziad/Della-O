#include "Common.h"
#include "Logger.h"
#include "String.h"
#include "Memory.h"

static char* ConvertLogTypeToString(Log_Type type) {
	char* tmp = "";
	switch (type) {
	case LOG_INFO:
		tmp = "INFO";
		break;
	case LOG_ERROR:
		tmp = "ERROR";
		break;
	case LOG_WARNING:
		tmp = "WARNING";
		break;
	}

	U32 length = GetStringLength(tmp);
	
	char* tmp_buffer = Malloc(sizeof(*tmp_buffer) * (length + 1));
	Memcpy(tmp_buffer, tmp, length);
	tmp_buffer[length] = '\0';
	
	return tmp_buffer;
}

void Log(Log_Type type, const char* fmt) {
	char* buffer = ConvertLogTypeToString(type);
	Print("[%s] %s", buffer, fmt);
	Free(buffer);
}