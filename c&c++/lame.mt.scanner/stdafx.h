// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <stdio.h>

#ifdef _WIN32

#include <tchar.h>
#include <Windows.h>

#else

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdarg.h>
#include <string.h>
#define stricmp		strcasecmp
#define strnicmp	strncasecmp

#endif //_WIN32

// TODO: reference additional headers your program requires here

#include <queue>
