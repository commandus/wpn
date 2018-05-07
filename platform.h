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

#define str_realpath(retval, path) \
{ \
	char full_path[PATH_MAX]; \
	retval = std::string(realpath(path.c_str(), full_path)); \
}

#endif

#ifdef __cplusplus
#define CALLC extern "C" 
#else
#define CALLC
#endif
