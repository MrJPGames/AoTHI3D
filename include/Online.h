#ifndef Online_H
#define Online_H

#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "SaveManager.h"

using namespace std;

class Online{
	public:
		Online();
		Result getLeaderboard();
		string getStringFromURL(const char* url, bool toPageData);
		bool isLoaded();
		void setLoaded(bool ld);
		string getPageData(int page_num, int page_line);
		int getPageLines();
		int getPageCount();
		void loadStatsFromFile(string fileLocation);
		void loadStatsFromBuffer(const char* buffer, int size);
		string getRawPageData();
	private:
		string pageData;
		string pages[6][11]; //Max 10 pages with pageSize 11 (page[x][0] is always title)
		int pageLines=10;
		int pageCount=1;
		bool loaded=false;
};
#endif