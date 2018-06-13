// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifdef _WIN32
#include <stdio.h>
#include <tchar.h>
#else
typedef long HRESULT;
typedef const char* LPCSTR;
typedef int BOOL;
#define E_FAIL				long(0x80004005L)
#define S_OK				long(0x0L)
#define stricmp				strcasecmp
#endif


// TODO: reference additional headers your program requires here
