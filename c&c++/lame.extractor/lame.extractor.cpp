// lame.extractor.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "../../include/lame.v2.h"
#include <string>
#include <vector>


int32_t  g_count = 0;
class LameExtractor
{
public:
	LameExtractor():m_vdb(0),
				    m_lame(0)
	{}
	~LameExtractor()
	{
		Unload();
	}
public:
	bool Load()
	{
		m_vdb = lame_open_vdb(0);
		if (!m_vdb) return false;


		m_lame = lame_create(m_vdb);
		if (!m_lame)  return false;

		if (LAME_FAILED(lame_init(m_lame))) return false;

#ifdef _WIN32
		CreateDirectoryA("dump" , 0);
#else
		mkdir( "dump", 0755 );
#endif

		return true;
	}

	bool ExtractFile(const char* file , const char * password)
	{
		if (!file || !m_lame) return false;
		lame_extract_feedback cbs;
		cbs.enter_file = Lame_Scan_Enter_File;
		cbs.leave_file = Lame_Scan_Leave_File;
		lame_extract_file(m_lame , file , password , &cbs , 0);
		return true;
	}

	void Unload()
	{
		if (m_vdb)
		{
			lame_close_vdb(m_vdb);
			m_vdb = 0;
		}

		if (m_lame)
		{
			lame_close_vdb(m_lame);
			m_lame = 0;
		}
	}
private:
#define MAX_BUF_SIZE		0x1000
	static bool GetFileName(const char* full_name , std::string& name)
	{
		if (!full_name) return false;

		const char* p = strrchr(full_name , ':');
		if (!p)
		{
			name.assign(full_name);
		}
		else
		{
			name.assign(p+1);
		}

		return true;
	}

	static  LXCT __stdcall Lame_Scan_Enter_File(const char* fname , const char* fmt , uint32_t depth , uint64_t flag, void* handle , void* userdata){
		if (!fname || !fmt || !handle) return LCT_DO_EXTRACT;

		long file_size = lame_file_get_size(handle);
		printf("[%s]:%s , size:%d , depth:%d\n" , fmt , fname , file_size , depth);

		std::string file_name;
		GetFileName(fname , file_name);

		std::string new_file_name("dump\\");
		new_file_name.append(file_name);

		/*FILE* file = fopen(new_file_name.c_str() , "wb");
		if (!file) return LCT_CONTINUE;

		uint8_t  buf[MAX_BUF_SIZE];
		while (true)
		{
		long real_read_count = lame_file_read(handle , buf , MAX_BUF_SIZE);
		if (!real_read_count) break;

		fwrite(buf , real_read_count , 1, file);
		}

		fclose(file);*/

		
		return LCT_DO_EXTRACT;
	}
	static  void __stdcall Lame_Scan_Leave_File(const char* fname , const char* fmt , uint32_t depth , uint64_t flag, void* handle , void* userdata , LXLVT lv)
	{

	}
private:
	void* m_vdb;
	void* m_lame;
public:
	std::string file_store_path;
	
};


int main(int argc, char* argv[])
{
	if (argc <= 1) return -1;
	
	LameExtractor _LameExtractor;
	if(!_LameExtractor.Load()) return -1;

	if (argc == 2)
	{
		_LameExtractor.ExtractFile(argv[1] , "");
	}
	else
	{
		_LameExtractor.ExtractFile(argv[1] , argv[2]);
	}

	

	_LameExtractor.Unload();

	return 0;
}

