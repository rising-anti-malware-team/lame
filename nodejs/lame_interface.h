#ifndef __Lame_Interface_H__
#define __Lame_Interface_H__

#include "../include/lame.v2.h"

#ifdef __cplusplus
extern "C" {
#endif

	typedef void*(*pfn_lame_open_vdb)(const char* vlibf);
	typedef void(*pfn_lame_close_vdb)(void* vdb);

	typedef void*(*pfn_lame_create)(void* vdb);
	typedef void(*pfn_lame_destroy)(void* lame);
	typedef void*(*pfn_lame_fork)(void* lame);

	typedef long(*pfn_lame_param_set)(void* lame, const char*  param);


	typedef long(*pfn_lame_init)(void* lame);
	typedef long(*pfn_lame_scan_file)(void* lame, const char* fname, lame_scan_result* pResult);
	typedef long(*pfn_lame_scan_file_w)(void* lame, const wchar_t* fname, lame_scan_result* pResult);
	typedef long(*pfn_lame_scan_mem)(void* lame, uint8_t* data, uint32_t size, lame_scan_result* pResult);


	typedef long(*pfn_lame_scan_file_with_callback)(void* lame, const char* fname, lame_scan_feedback* cbs, void* user_data);
	typedef long(*pfn_lame_scan_file_with_callback_w)(void* lame, const wchar_t* fname, lame_scan_feedback* cbs, void* user_data);
	typedef long(*pfn_lame_scan_mem_with_callback)(void* lame, uint8_t* data, uint32_t size, lame_scan_feedback* cbs, void* user_data);

	typedef long(*pfn_lame_get_version)(lame_info* info);
	typedef long(*pfn_lame_get_licence_info)(rx_licence_info* info);

	typedef long(*pfn_lame_extract_file)(void* lame, const char* fname, const char* password, lame_extract_feedback* cb, void* userdata);
	typedef long(*pfn_lame_extract_file_w)(void* lame, const wchar_t* fname, const wchar_t* password, lame_extract_feedback* cb, void* userdata);
	typedef long(*pfn_lame_file_get_size)(void* handle);
	typedef long(*pfn_lame_file_seek)(void* handle, int32_t offset, LSMETHOD method);
	typedef long(*pfn_lame_file_tell)(void* handle);
	typedef long(*pfn_lame_file_read)(void* handle, uint8_t* buf, uint32_t size);

#ifdef __cplusplus
}
#endif


#ifdef _WIN32

#include <windows.h>
#define LAME_PATH					"lame.dll"

#else

typedef int (CALLBACK *FARPROC)();
#define LAME_PATH					"lame.so"
#define CALLBACK					__stdcall
typedef long						HRESULT;
#define E_FAIL						long(0x80004005L)
#define S_OK						long(0x0L)
#define E_INVALIDARG				long(0x80070057L)

#endif //_WIN32

class CLameInterface
{
public:
	CLameInterface() : handle(NULL) {}
	~CLameInterface(){
		if (NULL != handle) {
			FreeLibrary(handle);
			handle = NULL;
		}
	}
	bool init() {
#ifdef _WIN32
		//char chCurDir[MAX_PATH] = { 0 };
		//GetCurrentDirectory(MAX_PATH, chCurDir);
		//SetDllDirectoryA(chCurDir);
		//handle = LoadLibraryA(LAME_PATH);
		//SetDllDirectoryA(NULL);
		handle = LoadLibraryExA(LAME_PATH, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
#else
		handle = LoadLibraryA(LAME_PATH);
#endif
		if (NULL == handle) return false;

		lame_open_vdb					= (pfn_lame_open_vdb) GetAddress("lame_open_vdb");
		lame_close_vdb					= (pfn_lame_close_vdb) GetAddress("lame_close_vdb");

		lame_create						= (pfn_lame_create) GetAddress("lame_create");
		lame_destroy					= (pfn_lame_destroy) GetAddress("lame_destroy");
		lame_fork						= (pfn_lame_fork) GetAddress("lame_fork");

		lame_param_set					= (pfn_lame_param_set) GetAddress("lame_param_set");

		lame_init						= (pfn_lame_init) GetAddress("lame_init");
		lame_scan_file					= (pfn_lame_scan_file) GetAddress("lame_scan_file");
		lame_scan_file_w				= (pfn_lame_scan_file_w) GetAddress("lame_scan_file_w");
		lame_scan_mem					= (pfn_lame_scan_mem) GetAddress("lame_scan_mem");
		lame_scan_file_with_callback	= (pfn_lame_scan_file_with_callback) GetAddress("lame_scan_file_with_callback");
		lame_scan_file_with_callback_w	= (pfn_lame_scan_file_with_callback_w) GetAddress("lame_scan_file_with_callback_w");
		lame_scan_mem_with_callback		= (pfn_lame_scan_mem_with_callback) GetAddress("lame_scan_mem_with_callback");

		lame_get_version				= (pfn_lame_get_version) GetAddress("lame_get_version");
		lame_get_licence_info			= (pfn_lame_get_licence_info) GetAddress("lame_get_licence_info");

		lame_extract_file				= (pfn_lame_extract_file) GetAddress("lame_extract_file");
		lame_extract_file_w				= (pfn_lame_extract_file_w) GetAddress("lame_extract_file_w");
		lame_file_get_size				= (pfn_lame_file_get_size) GetAddress("lame_file_get_size");
		lame_file_seek					= (pfn_lame_file_seek) GetAddress("lame_file_seek");
		lame_file_tell					= (pfn_lame_file_tell) GetAddress("lame_file_tell");
		lame_file_read					= (pfn_lame_file_read) GetAddress("lame_file_read");
		return true;
	}

private:
	FARPROC GetAddress(char* lpProcName) {
		if (NULL == handle) return NULL;
		return GetProcAddress(handle, lpProcName);
	}
#ifndef _WIN32
	HMODULE WINAPI LoadLibraryA( LPCSTR lpLibFileName ) {
		HMODULE h = dlopen(lpLibFileName, RTLD_LAZY);
		if (!h) {
//			printf("%s\n", dlerror());
		}
		return h;
	}
	FARPROC WINAPI GetProcAddress( HMODULE hModule, LPCSTR lpProcName ) {
		return (FARPROC)dlsym(hModule, lpProcName);
	}
	BOOL WINAPI FreeLibrary( HMODULE hLibModule ) {
		return dlclose(hLibModule);
	}
#endif //_WIN32

private:
	HMODULE handle;

public:
	pfn_lame_open_vdb lame_open_vdb;
	pfn_lame_close_vdb lame_close_vdb;
	pfn_lame_create lame_create;
	pfn_lame_destroy lame_destroy;
	pfn_lame_fork lame_fork;
	pfn_lame_param_set lame_param_set;
	pfn_lame_init lame_init;
	pfn_lame_scan_file lame_scan_file;
	pfn_lame_scan_file_w lame_scan_file_w;
	pfn_lame_scan_mem lame_scan_mem;
	pfn_lame_scan_file_with_callback lame_scan_file_with_callback;
	pfn_lame_scan_file_with_callback_w lame_scan_file_with_callback_w;
	pfn_lame_scan_mem_with_callback lame_scan_mem_with_callback;
	pfn_lame_get_version lame_get_version;
	pfn_lame_get_licence_info lame_get_licence_info;
	pfn_lame_extract_file lame_extract_file;
	pfn_lame_extract_file_w lame_extract_file_w;
	pfn_lame_file_get_size lame_file_get_size;
	pfn_lame_file_seek lame_file_seek;
	pfn_lame_file_tell lame_file_tell;
	pfn_lame_file_read lame_file_read;
};

#endif //__Lame_Interface_H__
