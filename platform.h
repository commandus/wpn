#ifndef PLATFORM_H
#define PLATFORM_H	1

#ifdef _WIN32

#include "windows.h"
#define sleep(seconds) \
Sleep(seconds *1000);

#define str_realpath(retval, path) \
{ \
	TCHAR full_path[MAX_PATH]; \
	GetFullPathNameA(path.c_str(), MAX_PATH, full_path, NULL); \
	retval = std::string(full_path); \
}

#else

#include <limits.h>
#include <stdlib.h>
#include <unistd.h>

#define INVALID_SOCKET		-1

#define str_realpath(retval, path) \
{ \
	char full_path[PATH_MAX]; \
	char *p = realpath(path.c_str(), full_path); \
	if (p) \
		retval = std::string(p); \
	else \
		retval = std::string(path); \
}

#endif

#ifdef __cplusplus
#define CALLC extern "C" 
#else
#define CALLC
#endif

#endif
