// stdafx.cpp : 只包括标准包含文件的源文件
// jlame.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO: 在 STDAFX.H 中
// 引用任何所需的附加头文件，而不是在此文件中引用
#ifndef _WIN32

// version
#define _RS_TOSTRING(x)	#x
#define RS_TOSTRING(x)	_RS_TOSTRING(x)

#ifdef TARGET_LBE
#define RS_ENDIAN		"Little-Endian "
#else
#define RS_ENDIAN		"Big-Endian "
#endif

#ifdef TARGET_64BIT
#define RS_BITS			"x64 "
#else
#define RS_BITS			"x86 "
#endif

#define RS_LAME_VERSION	"version: " RS_ENDIAN RS_BITS RS_TOSTRING(RS_FILEVERSION) " " RS_TOSTRING(RS_DATE) "\n"

const char* version = "\nlibjlame.so " RS_LAME_VERSION;

#endif
