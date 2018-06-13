#ifndef __SDK_TYPES_H__
#define __SDK_TYPES_H__

#define _OPTIONAL_

#ifdef __cplusplus
#define EXTERN_C    extern "C"
#else
#define EXTERN_C    extern
typedef char bool;
#define true	1
#define false	0
#endif

#include <stdint.h>


#define VIRUS_LENGTH 256
#define ENGID_LENGTH 32
#define HITAG_LENGTH 32
#define CLASS_NAME_LEN	0x10
#define ENGINE_VERSION_LEN	32
#define VIRUSDB_VERSIN_LEN	32

typedef enum rx_mk_t {
	//////内部使用///////
	mkTrait		= 0,
	mkHidden		= 1,
	mkHiddenFU	= 2,
	mkComplier	= 3,
	mkPacker		= 4,
	mkFormat		= 5,		// 文件格式
	mkAppType	= 6,
	mkTrusted	= 9,
	////////////////////////////

	mkMalware = 0x10,	//恶意

	mkTrojan,			//木马
	mkBackdoor,			//后门
	mkWorm,				//蠕虫
	mkRootkit,			//恶意驱动
	mkExploit,			//漏洞
	mkHackTool,			//黑客工具
	mkAdware,			//恶意广告
	mkStealer,			//窃取信息
	mkSpammer,			//垃圾邮件
	mkSpyware,			//间谍软件
	mkVirus,			//感染行
	mkJoke,				//玩笑
	mkJunk,				//垃圾推广
	mkPUA,				//可疑
	mkDownloader,		//下载器
	mkDropper,			//释放器
	/// 2015-12-21
	mkRansom,			//勒索
	mkHoax,				//欺骗
	mkRiskware,			//危险软件
	// 2016-6-7
	mkUnwanted,			//可疑
	mkMonetizer,		//套现

	mkMobileBase		= 0xC0,
	// Mobile
	//	摘自《移动互联网恶意代码描述规范》
	mkPayment = mkMobileBase,	//	恶意扣费
	mkPrivacy,				//	隐私窃取
	mkRemote, 				//	远程控制
	mkSpread, 				//	恶意传播
	mkExpense, 				//	资费消耗
	mkSystem, 				//	系统破坏
	mkFraud, 				//	诱骗欺诈
	mkRogue,				//	流氓行为


	mkAttention = 0xFE,		// 注意!
	mkTypeMax = 0x100,
} rx_mk_t;


enum{
	COMPOUND_SIZE = 0,
	MEST_DEPTH,
	TREAT_TIMES,
	PRECISE_FORMAT,
	KILL,
	MALWARE_PATH,
};


typedef enum rx_scan_opt_t {
	OPT_NO_COW		= 1,
	OPT_TREAT		= 2,
} rx_scan_opt_t;

typedef enum rx_treat_result_code_t {
	TREAT_FAIL_FIX = -1,
	TREAT_OK_DELETED = 1,
	TREAT_OK_FIXED = 2,
} rx_trc_t;

typedef struct rx_licence_info
{
	char			Version[16];
	char			Owner[128];
	char			Date[64];
	char			Authm[32];
	char			Data[2048];
} rx_licence_info;



#endif