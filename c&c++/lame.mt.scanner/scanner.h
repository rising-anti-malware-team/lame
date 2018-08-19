#ifndef __SCANNER_H__
#define __SCANNER_H__
#include "stdafx.h"
#include <mutex>
#include <thread>

#include "../../include/lame.v2.h"
typedef std::vector<std::string> t_path_list;

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


class safe_file_queue
{
public:
	safe_file_queue(){_queue_max_size = 100;}
public:
	bool push(std::string v , bool force = true)
	{
		if (v.empty()) return true;

		if (force)
		{
			while (true)
			{
				{
					std::lock_guard<std::recursive_mutex> locker(_locker);
					if(_file_queue.size() < _queue_max_size) 
					{
						_file_queue.push(v);
						return true;
					}
				}
				std::this_thread::sleep_for(std::chrono::microseconds(100));
			}
		}
		else
		{
			std::lock_guard<std::recursive_mutex> locker(_locker);
			if(_file_queue.size() >= _queue_max_size) return false;
			_file_queue.push(v);
		}

		return true;
	}

	bool pop(std::string& v)
	{
		std::lock_guard<std::recursive_mutex> locker(_locker);
		if (_file_queue.empty()) return false;
		
		v = _file_queue.front();
		_file_queue.pop();
		
		return true;
	}

private:
	std::queue<std::string> _file_queue;
	std::recursive_mutex _locker;
	uint32_t _queue_max_size;
};


static std::mutex  g_printf_locker;
inline void mtprintf(const char* fmt , ...)
{
	std::lock_guard<std::mutex> lock(g_printf_locker);

	va_list ap; 
	va_start(ap, fmt); 
	int n = vprintf(fmt, ap); 
	va_end(ap); 
}

void logger_scan_result(const char* msg , lame_scan_result* result)
{
	std::lock_guard<std::mutex> lock(g_printf_locker);

	RSSetColor::SetConsoleColor(RSSetColor::green);
	printf("%s" , msg);
	if (result)
	{
		RSSetColor::SetConsoleColor(RSSetColor::red);
		printf("	Infected:%s (%s)" , result->vname , result->engid);
	}
	printf("\n");
}


class travel_dir
{
public:
	travel_dir(){}
public:
	void run(t_path_list& path_list)
	{
		m_thread = std::thread( std::bind(&travel_dir::Scan , this , path_list) );
	}
	void wait()
	{
		m_thread.join();
	}
	virtual void DoMeetFile( const char* path ) = 0;
private:
	void Scan(t_path_list& path_list)
	{
		for( size_t i = 0; i < path_list.size(); ++ i )
		{
			ScanMix( path_list[i].c_str() );
		}
	}
	void ScanMix( const char* path )
	{
#ifdef WIN32
		DWORD dwFileAttr = GetFileAttributes(path);
		if( dwFileAttr == INVALID_FILE_ATTRIBUTES ) return ;
		if( dwFileAttr & FILE_ATTRIBUTE_REPARSE_POINT ) return ;
		if( dwFileAttr & FILE_ATTRIBUTE_DIRECTORY ) ScanDir(path );
		else DoMeetFile(path);
#else

		struct stat statInfo;
		if(stat(path, &statInfo) != 0) return;
		if(S_ISREG(statInfo.st_mode))
			DoMeetFile(path);
		else if(S_ISDIR(statInfo.st_mode))
			ScanDir(path);
#endif
	}
	void ScanDir(const char* lpdir)
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
			else DoMeetFile(strDir.c_str());

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
			ScanMix(filePath.c_str());
		}
		closedir(dirp);
		return ;
#endif
	}
private:
	std::thread m_thread;
};

class travel_dir_worker : public travel_dir
{
public:
	travel_dir_worker(safe_file_queue& fq):m_fq(fq){}
public:
	void DoMeetFile( const char* path )
	{
		m_fq.push(path);
	}
private:
	safe_file_queue& m_fq;
};

class travel_dir_counter : public travel_dir
{
public:
	travel_dir_counter(uint64_t& cnt):m_filecnt(cnt){}
public:
	void DoMeetFile( const char* path )
	{
		std::lock_guard<std::recursive_mutex> locker(_locker);
		m_filecnt++;
	}
private:
	std::recursive_mutex _locker;
	uint64_t& m_filecnt;
};


class scan_workers
{

public:
	scan_workers(safe_file_queue& fq, uint64_t& filecnt):m_fq(fq),m_filecnt(filecnt),m_curfilecnt(0)
	{

	}
	~scan_workers()
	{
		for (uint32_t idx = 0 ; idx < m_lames.size() ; idx++)
		{
			lame_destroy(m_lames[idx]);
		}
	}
public:
	bool run(void* vlib ,t_path_list& params  ,uint32_t workers)
	{
		if (!vlib) return false;

		m_lames.resize(workers);
		m_workers.resize(workers);

		for (uint32_t idx = 0 ; idx < workers ; idx++)
		{	
			void* lame = lame_create(vlib);
			if (!lame) return false;

			for (uint32_t i = 0 ; i < params.size() ; i++)
			{
				lame_param_set(lame , params[i].c_str());
			}

			if (LAME_FAILED(lame_init(lame)))
			{
				lame_destroy(lame);
				return false;
			}

			m_lames[idx] = lame;
			m_workers[idx] = std::thread(&scan_workers::scan ,this ,lame);
 		}
		
		return true;
	}
	void wait()
	{
		for (uint32_t idx = 0 ; idx < m_workers.size() ; idx++)
		{
			m_workers[idx].join();
		}
	}
private:
	void scan(void* lame)
	{
		if (!lame) return;

		while (true)
		{
			std::string path;
			if (!m_fq.pop(path))
			{
				std::this_thread::sleep_for(std::chrono::microseconds(100));
				continue;
			}

			if (stricmp(path.c_str() , "exit") == 0)
			{
				m_fq.push("exit");
				break;
			}

			lame_scan_result result;
			long l = lame_scan_file(lame , path.c_str() , &result);
			//logger_scan_result(path.c_str() , l == 0 ? &result : 0);
			{
				std::lock_guard<std::recursive_mutex> locker(_locker);
				m_curfilecnt++;
				RSSetColor::SetConsoleColor(RSSetColor::green);
				printf("[%lld/%lld] %s", m_curfilecnt, m_filecnt, path.c_str());
				if(l == 0)
				{
					RSSetColor::SetConsoleColor(RSSetColor::red);
					printf("	Infected:%s (%s)" , result.vname , result.engid);
				}
				printf("\n");
			}
		}
	}
private:
	safe_file_queue& m_fq;
	std::vector<std::thread> m_workers;
	std::vector<void*> m_lames;
	std::recursive_mutex _locker;
	uint64_t& m_filecnt;
	uint64_t m_curfilecnt;
};



class lame_scanner
{
public:
	lame_scanner():m_vlib(0),
				   m_workers(0),
				   m_travel_worker(0),
				   m_filecnt(0),
				   m_travel_counter(0)
	{}
	~lame_scanner()
	{
		clear();
	}
public:
	void scan(t_path_list& params , t_path_list& path_list , uint32_t workers)
	{
		m_vlib = lame_open_vdb(0);
		if (!m_vlib) return;

		m_workers = new scan_workers(m_file_queue, m_filecnt);
		m_travel_worker = new travel_dir_worker(m_file_queue);
		m_travel_counter = new travel_dir_counter(m_filecnt);
		
		if (!m_workers || !m_travel_worker || !m_travel_counter)
		{
			clear();
			return;
		}

		m_travel_counter->run(path_list);
		m_travel_worker->run(path_list);
		m_workers->run(m_vlib , params , workers);

		m_travel_counter->wait();
		m_travel_worker->wait();
		m_file_queue.push("exit");
		m_workers->wait();

	}
	void clear()
	{
		if (m_workers)
		{
			delete m_workers;
			m_workers = 0;
		}

		if (m_travel_worker)
		{
			delete m_travel_worker;
			m_travel_worker = 0;
		}

		if (m_travel_counter)
		{
			delete m_travel_counter;
			m_travel_counter = 0;
		}

		if (m_vlib)
		{
			lame_close_vdb(m_vlib);
			m_vlib = 0;
		}
	}
private:
	void* m_vlib;
	scan_workers* m_workers;
	travel_dir_worker* m_travel_worker;
	travel_dir_counter* m_travel_counter;
	safe_file_queue m_file_queue;
	uint64_t m_filecnt;
};

#endif