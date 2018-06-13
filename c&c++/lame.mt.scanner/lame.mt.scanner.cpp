// lame.mt.scanner.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "scanner.h"


int main(int argc, const char* argv[])
{
	if (argc <= 1) return -1;

	uint32_t workers = 2;
	std::vector<std::string>  params;
	std::vector<std::string>  path_list;
	for (int idx = 1; idx < argc ; idx++)
	{
		if (strnicmp(argv[idx] , "-workers=" , 9) == 0)
		{
			if (strlen(argv[idx]) == 9) continue;
			std::string s(argv[idx]+9);
			uint32_t vl = atoi(argv[idx]+9);
			if (vl > 0) workers = vl;
		}
		else if (strnicmp(argv[idx] , "-" , 1) == 0)
		{
			if (strlen(argv[idx]) == 1) continue;
			params.push_back(argv[idx]);
		}
		else
		{
			path_list.push_back(argv[idx]);
		}
	}
	
	lame_scanner _scanner;
	_scanner.scan(params , argv[1] , workers);

	return 0;
}

