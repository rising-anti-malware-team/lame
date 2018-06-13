#ifndef __LAME_V2_H__
#define __LAME_V2_H__

#include "sdk.types.h"

typedef struct lame_scan_result
{
	rx_mk_t			mklass;
	char			kclass[CLASS_NAME_LEN];
	char			kclass_desc_a[CLASS_NAME_LEN];
	wchar_t			kclass_desc_w[CLASS_NAME_LEN];
	char			engid[ENGID_LENGTH];
	char			vname[VIRUS_LENGTH];
	uint32_t		vid32;
	uint64_t		vid40;
	char			hitag[HITAG_LENGTH];
	rx_trc_t		treat;
}lame_scan_result;


typedef enum LSMETHOD_ 
{
	LAME_SEEK_SET = 0,
	LMAE_SEEK_CUT = 1,
	LMAE_SEEK_TAIL =2
}LSMETHOD;


typedef enum LSCT_
{
	LSCT_CONTINUE = 0x01,
	LSCT_ABORT
}LSCT;


typedef enum LXCT_
{
	LCT_CONTINUE = 0x01,
	LCT_DO_EXTRACT,
	LCT_ABORT,

}LXCT;


typedef enum LXLVT_
{
	LXLVT_NORMAL = 0x01,
	LXLVT_ABORT  = 0x02,
	LXLVT_ERROR  = 0x03
}LXLVT;

typedef struct lame_info_
{
	char  engv[ENGINE_VERSION_LEN+1];
	char  vdbv[VIRUSDB_VERSIN_LEN+1];
}lame_info;



enum 
{
	FCLS_PROGRAM	= 0x00000010,	// DOSCOM/PE//ELF/JAVA
	FCLS_DOCUMENT   = 0x00000020,	// PDF/DOC/DOCX/rtf
	FCLS_PACKAGE	= 0x00000040,	// Zip/7z/...
	FCLS_MMEDIA		= 0x00000080,	// Image|Moive|Audio
	FCLS_SCRIPT		= 0x00000100,	// All script
	FCLS_EMAIL		= 0x00000200,	// E-Mail
	FCLS_DISKIMG	= 0x00000400,	// Disk in file, VHD, VMDK, ...
	FCLS_BOOT		= 0x00000800,	// Virtual Disk
};

#define LAMECALLTYPE      __cdecl
#define LAMESTDCALLTYPE      __stdcall

#ifndef _WIN32
#define __cdecl
#define __stdcall
#endif

typedef LSCT  (LAMESTDCALLTYPE *lame_scan_enter_file)(const char* file_name , uint32_t depth , void* usr_data);
typedef void  (LAMESTDCALLTYPE *lame_scan_leave_file)(const char* file_name , uint32_t depth ,void* usr_data , LXLVT l);
typedef LSCT  (LAMESTDCALLTYPE *lame_scan_alarm)(const char* file_name , lame_scan_result* info,void* usr_data);

typedef LXCT  (LAMESTDCALLTYPE *lame_extract_enter_file)(const char* file_name , const char* format , uint32_t depth , uint64_t FCLS ,void* hanlde , void* userdata);
typedef void  (LAMESTDCALLTYPE *lame_extract_leave_file)(const char* file_name , const char* format , uint32_t depth , uint64_t FCLS ,void* hanlde , void* userdata , LXLVT l);


typedef struct lame_scan_feedback_
{
	lame_scan_enter_file enter_file;
	lame_scan_leave_file leave_file;
	lame_scan_alarm		 alarm;
}lame_scan_feedback;

typedef struct lame_extract_feedback_
{
	lame_extract_enter_file enter_file;
	lame_extract_leave_file leave_file;
}lame_extract_feedback;

#define LAME_SUCCEEDED(hr)   (((long)(hr)) >= 0)
#define LAME_FAILED(hr)      (((long)(hr)) < 0)

EXTERN_C void*  lame_open_vdb(const char* vlibf);
EXTERN_C void   lame_close_vdb(void* vdb);

EXTERN_C void*  lame_create(void* vdb);
EXTERN_C void   lame_destroy(void* lame);
EXTERN_C void*  lame_fork(void* lame);

EXTERN_C long   lame_param_set(void* lame , const char*  param);


EXTERN_C long   lame_init(void* lame);
EXTERN_C long   lame_scan_file(void* lame , const char* fname , lame_scan_result* pResult) ;
EXTERN_C long   lame_scan_file_w(void* lame , const wchar_t* fname , lame_scan_result* pResult) ;
EXTERN_C long   lame_scan_mem(void* lame , uint8_t* data , uint32_t size , lame_scan_result* pResult);


EXTERN_C long   lame_scan_file_with_callback(void* lame , const char* fname , lame_scan_feedback* cbs , void* user_data) ;
EXTERN_C long   lame_scan_file_with_callback_w(void* lame , const wchar_t* fname , lame_scan_feedback* cbs , void* user_data) ;
EXTERN_C long   lame_scan_mem_with_callback(void* lame , uint8_t* data , uint32_t size , lame_scan_feedback* cbs , void* user_data);

EXTERN_C long   lame_get_version(lame_info* info);
EXTERN_C long   lame_get_licence_info(rx_licence_info* info);



EXTERN_C long	lame_extract_file(void* lame , const char* fname , const char* password  , lame_extract_feedback* cb , void* userdata);
EXTERN_C long	lame_extract_file_w(void* lame , const wchar_t* fname , const wchar_t* password , lame_extract_feedback* cb , void* userdata);
EXTERN_C long	lame_file_get_size(void* handle);
EXTERN_C long	lame_file_seek(void* handle , int32_t offset , LSMETHOD method);
EXTERN_C long	lame_file_tell(void* handle);
EXTERN_C long	lame_file_read(void* handle , uint8_t* buf , uint32_t size);
#endif