#ifndef SM_H
#define SM_H

#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <sstream>
#include "TinyXML2.h"

using namespace std;

class AoTSJSaveManager{
	public:
		void loadSaveDataFromFile(string fileLocation);
		void storeSaveData(string fileLocation);
		int getScore(int difficulty);
		int getItemsDestroyed(int difficulty);
		string getName(int difficulty);
		inline bool file_exists(const std::string& name);
		void setScore(int dif, int scr);
		void setItemsDestroyed(int dif, int idstry);
		void setPlayerSkin(int skinID);
		int getPlayerSkin();
	private:
		tinyxml2::XMLDocument xml_doc;
		string localUsername;
		string name[3];
		int score[3]={0,0,0};
		int itemsDestroyed[3] = {0,0,0};
		int skin=0;
};
#endif