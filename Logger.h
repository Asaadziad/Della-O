#pragma once

typedef enum {
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR,
	LOG_TYPE_COUNT,
} Log_Type;

void Log(Log_Type type, const char* fmt);

#ifndef LOG_FUNCTIONS
#define LOG_FUNCTIONS

#define LOG_INFO(msg) Log(LOG_INFO, msg)
#define LOG_WARNING(msg) Log(LOG_WARNING, msg)
#define LOG_ERROR(msg) Log(LOG_ERROR, msg)

#endif

static void panic(const char* msg, const char* file, int line, ...) {
	Print("[%s, %d]: ", file, line);
	va_list args;
	va_start(args, line);
	Print(msg, args);
	va_end(args);
	Print("\n");
	exit(1);
}

#define PANIC(msg, ...) panic(msg,__FILE__, __LINE__, ##__VA_ARGS__ ) 