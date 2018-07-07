// scan.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <sys/socket.h> 
#include <sys/epoll.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <netinet/in.h> 
#include <netinet/tcp.h> 
#include <arpa/inet.h>
#include <netdb.h>
#include <ctype.h>
#include <fcntl.h> 
#include <unistd.h>
#include <pthread.h>
#include <errno.h> 
#include <time.h> 
#include <signal.h>
#include <dirent.h>
#else
#include <Windows.h>
#endif
#include "../../include/lame.h"
#include "../../include/lame.v2.h"
#include <string>
#include <vector>
#include <time.h>
#include <exception>

namespace RSSetColor
{
	enum color_style
	{
		blue	= 0x0001,
		green	= 0x0002,
		red		= 0x0004,
		rg		= red | green,
		rb		= red | blue,
		gb		= green | blue,
		rgb		= blue | green | red,
	};


#ifdef _WIN32

	static inline void SetConsoleColor( int color )
	{
		WORD wAttributes = FOREGROUND_INTENSITY;
		if ( color & blue ) wAttributes |= FOREGROUND_BLUE;
		if ( color & red ) wAttributes |= FOREGROUND_RED;
		if ( color & green ) wAttributes |= FOREGROUND_GREEN;
		SetConsoleTextAttribute( GetStdHandle(STD_OUTPUT_HANDLE), wAttributes );
	}

#else

#define NONE                 "\e[0m"
#define BLACK                "\e[0;30m"
#define L_BLACK              "\e[1;30m"
#define RED                  "\e[0;31m"
#define L_RED                "\e[1;31m"
#define GREEN                "\e[0;32m"
#define L_GREEN              "\e[1;32m"
#define BROWN                "\e[0;33m"
#define YELLOW               "\e[1;33m"
#define BLUE                 "\e[0;34m"
#define L_BLUE               "\e[1;34m"
#define PURPLE               "\e[0;35m"
#define L_PURPLE             "\e[1;35m"
#define CYAN                 "\e[0;36m"
#define L_CYAN               "\e[1;36m"
#define GRAY                 "\e[0;37m"
#define WHITE                "\e[1;37m"

#define BOLD                 "\e[1m"
#define UNDERLINE            "\e[4m"
#define BLINK                "\e[5m"
#define REVERSE              "\e[7m"
#define HIDE                 "\e[8m"
#define CLEAR                "\e[2J"
#define CLRLINE              "\r\e[K" //or "\e[1K\r"

	static inline  void SetConsoleColor( int color )
	{
		char* lpAttributes = NULL;
		switch (color)
		{
		case red:	lpAttributes = RED; break; //"\033[22;31m";
		case green:	lpAttributes = GREEN; break; //"\033[22;32m";
		case blue:	lpAttributes = BLUE; break; //"\033[22;31m";
		case rgb:	lpAttributes = WHITE; break; //"\033[01;37m";			
		case rg:	lpAttributes = YELLOW; break;
		case rb:	lpAttributes = PURPLE; break;
		case gb:	lpAttributes = CYAN; break;
		default:	lpAttributes = NONE; break;
		}
		printf( lpAttributes );
	}

#endif

};


struct IScanner
{
	virtual ~IScanner(){};
	virtual bool Load() = 0;
	virtual bool SetParam(const char* param)= 0;
	virtual bool Init() = 0;
	virtual void Scan(LPCSTR file) = 0;
	virtual void ScanWithCallback(LPCSTR file) = 0;
	virtual void ShowEngineInfo() = 0;
	virtual void ShowLicenseInfo() = 0;
	virtual void UnInit() = 0;
	virtual void ShowStatistics()=0;
};

class LameScannerV1: public IScanner
{
public:
	LameScannerV1():m_file_count(0),
				   m_virus_count(0)
	{

	}
	~LameScannerV1()
	{
		UnInit();
    }
public:
	bool Load()
	{
		
		rx_licence_info info;
		rx_lame_get_licence_info(&info , "licence.json");

		conf = rx_obj_create(COBJ_CONF);
		if (!conf){
			printf("rx_obj_create(COBJ_CONF) failed.\n");
			return false;
		}

		rx_conf_set_int(conf, "cloud", 1);

		lame = rx_obj_create(COBJ_LAME);
		if (!lame)
		{
			printf("rx_obj_create(COBJ_LAME) failed.\n");
			rx_obj_release(conf);
			return false;
		}

		if(rx_lame_init(lame, conf) != 0)
		{
			printf("rx_lame_init failed.\n");
			rx_obj_release(conf);
			rx_obj_release(lame);
			return false;
		}

		return true;
	}
	bool SetParam(const char* param)
	{
		return false;
	}
	bool Init()
	{
		return true;
	}
	void Scan(LPCSTR file)
	{
		if (!file) return;
		m_file_count++;
		RSSetColor::SetConsoleColor(RSSetColor::green);
		printf("%s" , file);

		rx_result_t result;
		rx_sdr_t sdr;

		sdr = rx_lame_scan(lame, file, OPT_NO_COW);
		if( rx_sdr_get_result(sdr, &result) )
		{
			m_virus_count++;
			RSSetColor::SetConsoleColor(RSSetColor::red);
			printf("	Infected:%s\n", file , result.vname );
		}
			
		if( sdr ) rx_obj_release( sdr );

		printf("\n");
	}
	void ScanWithCallback(LPCSTR file)
	{
		return;
	}
	void ShowEngineInfo()
	{
		char ver[1024] = {};
		rx_lame_get_virus_library_version(ver, sizeof(ver), NULL);
		printf("viruslib version: %s\n", ver);
	}
	void ShowLicenseInfo()
	{
		rx_licence_info info = {0};
		rx_lame_get_licence_info(&info, NULL);
		printf("licence info:\n%s\n%s\n%s\n%s\n%s\n", info.Version, info.Owner, info.Date, info.Authm, info.Data );
	}
	void ShowStatistics()
	{
		printf("Files: %I64d / %I64d = %f\n",m_virus_count ,m_file_count , m_virus_count == 0 ? 0:(double)(m_virus_count)/m_file_count);
	}
	void UnInit()
	{
		if (conf)
		{
			rx_obj_release(conf);
			conf = 0;
		}

		if (lame)
		{
			rx_obj_release(lame);
			lame = 0;
		}
		
	}
private:
	rx_obj_t conf;
	rx_obj_t lame;
	uint64_t m_file_count;
	uint64_t m_virus_count;
};

class LameScannerV2: public IScanner
{
public:
	LameScannerV2():m_vdb(0),
					m_lame(0),
					m_file_count(0),
					m_virus_count(0)
	{}
	~LameScannerV2()
	{
		UnInit();
	}
public:
	bool Load()
	{
		m_vdb = lame_open_vdb(0);
		if (!m_vdb) return false;


		m_lame = lame_create(m_vdb);
		if (!m_lame)  return false;

		return true;
	}
	bool SetParam(const char* param)
	{
		if (!m_lame || !param) return false;
		if (!LAME_SUCCEEDED(lame_param_set(m_lame , param))) return false;
		return true;
	}
	bool Init()
	{
		if (!m_lame) return false;
		if (LAME_FAILED(lame_init(m_lame))) return false;
#if 1
		//测试fork接口
		void* lame = lame_fork(m_lame);
		lame_destroy(m_lame);
		m_lame = lame;

#endif
	
		return true;
	}
	void Scan(LPCSTR file)
	{
		if (!file) return;
		m_file_count++;
		RSSetColor::SetConsoleColor(RSSetColor::green);
		printf("%s" , file);
		

		lame_scan_result result;
		
		long l = lame_scan_file(m_lame , file , &result);
		if(LAME_SUCCEEDED(l))
		{
			m_virus_count++;
			RSSetColor::SetConsoleColor(RSSetColor::red);
			printf("	Infected:%s (%s)" , result.vname, result.engid);
		}
		printf("\n");
	}
	void ScanWithCallback(LPCSTR file)
	{
		if (!file) return;
		m_file_count++;
		lame_scan_feedback cbs;
		
		cbs.enter_file = lame_scan_enter_file;
		cbs.leave_file = lame_scan_leave_file;
		cbs.alarm = Lame_Scan_Alarm;

		lame_scan_file_with_callback(m_lame , file , &cbs , this);
		return;
	}
	void ShowEngineInfo()
	{
		
		lame_info info;
		if (LAME_SUCCEEDED(lame_get_version(&info)))
		{
			printf("Scanner:\n");
			printf("Engine Version : %s\n" , info.engv);
			printf("Malware Version: %s\n" , info.vdbv);
		}
	}
	void ShowLicenseInfo()
	{
		rx_licence_info info;
		if (LAME_SUCCEEDED(lame_get_licence_info(&info)))
		{
			printf("Version: %s\n" , info.Version);
			printf("Owner:	%s\n" , info.Owner);
			printf("Date:	%s\n" , info.Data);
			printf("Authm:	%s\n" , info.Authm);
		}
	}
	void ShowStatistics()
	{
		 printf("Files: %I64d / %I64d = %f\n",m_virus_count ,m_file_count , m_virus_count == 0 ? 0:(double)(m_virus_count)/m_file_count);
	}
	void UnInit()
	{
		if (m_vdb)
		{
			lame_close_vdb(m_vdb);
			m_vdb = 0;
		}

		if (m_lame)
		{
			lame_destroy(m_lame);
			m_lame = 0;
		}
	}
private:
	static LSCT  __stdcall lame_scan_enter_file(const char* file_name , uint32_t depth , void* usr_data)
	{
		RSSetColor::SetConsoleColor(RSSetColor::green);
		printf("%s" , file_name);
		return LSCT_CONTINUE;
	}
	static void  __stdcall lame_scan_leave_file(const char* file_name , uint32_t depth ,void* usr_data , LXLVT l)
	{
		printf("\n");
	}
	static  LSCT __stdcall Lame_Scan_Alarm(const char* fname , lame_scan_result* result , void* user_data)
	{
		LameScannerV2* lv2 = (LameScannerV2*)user_data;
		if (!lv2) return LSCT_ABORT;

		lv2->m_virus_count++;
		RSSetColor::SetConsoleColor(RSSetColor::red);
		printf("	Infected:%s (%s)" , result->vname, result->engid);

		return LSCT_CONTINUE;
	}
	
private:
	void* m_vdb;
	void* m_lame;
public:
	uint64_t m_file_count;
	uint64_t m_virus_count;
};

class DTravel
{
public:
	DTravel(IScanner* scanner , BOOL showlist):m_scanner(scanner),m_show_file_list(showlist){}
public:
	void Scan(LPCSTR lpdir)
	{

#ifdef _WIN32

		DWORD dw = GetFileAttributes(lpdir);
		if( dw == INVALID_FILE_ATTRIBUTES ) return ;
		if( dw & FILE_ATTRIBUTE_REPARSE_POINT ) return ;
		if( dw & FILE_ATTRIBUTE_DIRECTORY ) ScanDir(lpdir );
		else ScanFile(lpdir);
#else

		struct stat statInfo;
		if(stat(lpdir, &statInfo) != 0) return;
		if(S_ISREG(statInfo.st_mode))
			ScanFile(lpdir);
		else if(S_ISDIR(statInfo.st_mode))
			ScanDir(lpdir);
#endif
	}
private:
	void ScanDir(LPCSTR lpdir)
	{

#ifdef _WIN32
		HANDLE hFind;
		WIN32_FIND_DATA wfData;

		std::string sPath(lpdir);
		if (sPath.back() == '\\') sPath.append("*");
		else sPath.append("\\*");

		hFind = FindFirstFile(sPath.c_str(),&wfData);
		if(hFind == INVALID_HANDLE_VALUE) return ;

		do
		{
			if( wfData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT ) 
				continue;

			if( (_stricmp(wfData.cFileName,".")==0) || (_stricmp(wfData.cFileName,"..")==0) )
				continue;

			std::string strDir(lpdir);
			if (strDir.back() != '\\') strDir.append("\\");
			strDir.append(wfData.cFileName);

			if(wfData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ScanDir(strDir.c_str());
			else ScanFile(strDir.c_str());

		}while(FindNextFile(hFind,&wfData));

		FindClose(hFind);

#else

		DIR *dirp;  
		struct dirent *direntp;  
		if((dirp = opendir(lpdir)) == NULL)
			return;
		std::string filePath;
		while((direntp = readdir(dirp)) != NULL) {
			if(strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0)
				continue;
			filePath = lpdir;
			filePath.append("/");
			filePath.append(direntp->d_name);
			ScanFile(filePath.c_str());
		}
		closedir(dirp);
		return ;


#endif

	}
	HRESULT ScanFile(LPCSTR lpFileName)
	{
		if (!m_scanner) return E_FAIL;
		if (m_show_file_list)
		{
			m_scanner->ScanWithCallback(lpFileName);
		}
		else
		{
			m_scanner->Scan(lpFileName);
		}
		
		return S_OK;
	}
private:
	IScanner* m_scanner;
	BOOL  m_show_file_list;
};


static void ShowHelp()
{
	printf("Usage:\n");
	printf("	  -version			: show scanner info.\n");
	printf("	  -license			: show scanner license.\n");
	printf("	  -lame.v1			: use  scanner of V1 to scan,default V2\n");
	printf("	  -hold				: hold , press return to exit.\n");
	printf("	  -show-file-list	: show internal files of scan.\n\n\n");

	printf("Example:");
	printf("	  : lame.scanner d:\\samples");
	printf("	  : lame.scanner -lame.v1 -hold d:\\samples\n");
}



int main(int argc, char* argv[])
{
	int ret = -1;
	bool bShowVersion = false;
	bool bShowLicense = false;
	bool bUseLameV1 = false;
	bool bHoldon = false;
	bool bShowFileList = false;

	std::vector<std::string> params;
	std::vector<std::string> files;

	IScanner* scanner = 0;

	setlocale( LC_ALL, "" );
	RSSetColor::SetConsoleColor(RSSetColor::green);

	if (argc == 1) goto __exit__;

	for (uint32_t i = 1 ; i < argc ; i++)
	{
		if (*argv[i] == '-')
		{
			if (stricmp(argv[i] , "--help") == 0){ goto __exit__; }
			else if (stricmp(argv[i] , "-version") == 0){ bShowVersion = true;}
			else if (stricmp(argv[i] , "-license") == 0){ bShowLicense = true;}
			else if (stricmp(argv[i] , "-lame.v1") == 0){ bUseLameV1 = true;}
			else if (stricmp(argv[i] , "-hold") == 0){ bHoldon = true;}
			else if (stricmp(argv[i] , "-show-file-list") == 0){ bShowFileList = true;}
			else if (stricmp(argv[i] , "-xml") == 0){ bShowFileList = true;}
			else if (stricmp(argv[i] , "-selicen") == 0){ bShowFileList = true;}
			else
			{
				if (strlen(argv[i]) == 1) continue;
				params.push_back(argv[i]+1);
			}
		}
		else
		{
			files.push_back(argv[i]);
		}
	}

	if (bUseLameV1)
	{ 
		scanner = new LameScannerV1();
	}
	else
	{
		scanner = new LameScannerV2();
	}

	if (!scanner)
	{
		printf("Faild to create lame scanner.\n");
		goto __exit__;
	}

	if (!scanner->Load())
	{
		printf("Faild to load lame scanner.\n");
		goto __exit__;
	}

	//设置参数
	for (uint32_t i = 0 ; i < params.size() ;i++)
	{
		scanner->SetParam(params[i].c_str());
	}

	if (!scanner->Init())
	{
		printf("Faild to init lame scanner.\n");
		goto __exit__;
	}

	if (bShowVersion)
	{
		scanner->ShowEngineInfo();
	}

	if (bShowLicense)
	{
		scanner->ShowLicenseInfo();
	}

	if (files.size() > 0)
	{
		clock_t startTime = clock();
		DTravel _DTravel(scanner , bShowFileList);
		for (uint32_t i = 0 ; i < files.size() ; i++)
		{
			_DTravel.Scan(files[i].c_str());
		}
		clock_t endTime = clock();
		clock_t totalTime = endTime - startTime;
		double totalSec = 1.0 * totalTime / CLOCKS_PER_SEC;
		size_t _seconds = size_t(totalSec);
		size_t _mseconds = (totalSec - _seconds) * 1000;
		size_t _minutes = _seconds / 60; _seconds = _seconds % 60;
		printf("\n");
		scanner->ShowStatistics();
		printf("Elapse: %d:%d:%d(m:s:ms)\n", _minutes, _seconds, _mseconds );
	}
	ret = 0;

__exit__:
	if (-1 == ret) ShowHelp();
	if (scanner) delete scanner; scanner = 0;
	if (bHoldon)
	{
		getchar();
	}

	return ret;
}
