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
		Result getLeaderboardFromURL(const char* url);
		Result uploadLeaderBoardData(const char* url); //Does this when getLeaderbaard is requested as well.
		bool isLoaded();
		void setLoaded(bool ld);
		string getPageData(int page_num, int page_line);
		int getPageLines();
		int getPageCount();
		void convertDataToPages();
		string getRawPageData();
	private:
		string pageData;
		string pages[10][15]; //Max 10 pages with pageSize 15 (page[x][0] is always title)
		int pageLines=10;
		int pageCount=1;
		bool loaded=false;
};
#endif