#include "SaveManager.h"

using namespace std;

inline bool AoTSJSaveManager::file_exists(const std::string& name) {
	ifstream f(name.c_str());
	return f.good();
}

void AoTSJSaveManager::loadSaveDataFromFile(string fileLocation){
	//Load local username!
	cfguInit();
	u8 * outdata = (u8*)malloc(0x1C);
	u8 * outdata2 = (u8*)malloc(0x1C);
	CFGU_GetConfigInfoBlk2 (0x1C, 0x000A0000, outdata);
	utf16_to_utf8(outdata2,(u16*)outdata,0x1C);
	string str((char*)outdata2, 0x1B);
	localUsername = str;
	free(outdata);
	free(outdata2);
	cfguExit();

	if (file_exists(fileLocation)){
		//Load file
		std::ifstream t(fileLocation);
		std::stringstream buffer;
		buffer << t.rdbuf();

		tinyxml2::XMLDocument xml_doc;

		tinyxml2::XMLError eResult = xml_doc.Parse( buffer.str().c_str() );
		if (eResult == tinyxml2::XML_SUCCESS){
			tinyxml2::XMLNode* root = xml_doc.FirstChild();
			if (root != nullptr){
				tinyxml2::XMLElement* element = root->FirstChildElement("easy");
				if (element != nullptr){
					name[0]=element->FirstChildElement("name")->GetText();
					element->FirstChildElement("score")->QueryIntText(&score[0]);
					element->FirstChildElement("itemsDestroyed")->QueryIntText(&itemsDestroyed[0]);
				}
				element = root->FirstChildElement("normal");
				if (element != nullptr){
					name[1]=element->FirstChildElement("name")->GetText();
					element->FirstChildElement("score")->QueryIntText(&score[1]);
					element->FirstChildElement("itemsDestroyed")->QueryIntText(&itemsDestroyed[1]);
				}
				element = root->FirstChildElement("hard");
				if (element != nullptr){
					name[2]=element->FirstChildElement("name")->GetText();
					element->FirstChildElement("score")->QueryIntText(&score[2]);
					element->FirstChildElement("itemsDestroyed")->QueryIntText(&itemsDestroyed[2]);
				}
				element = root->FirstChildElement("general");
				if (element != nullptr){
					element->FirstChildElement("skinID")->QueryIntText(&skin);
				}
			}
		}
	}else{
		name[0]=localUsername;
		name[1]=localUsername;
		name[2]=localUsername;
		storeSaveData(fileLocation);
	}
}

void AoTSJSaveManager::storeSaveData(string fileLocation){
	tinyxml2::XMLDocument xml_doc;
	//Create root element
	tinyxml2::XMLNode * pRoot = xml_doc.NewElement("save");
	xml_doc.InsertFirstChild(pRoot);

	//Create difficulty elements
	tinyxml2::XMLNode * pEasyNode = xml_doc.NewElement("easy");
	pRoot->InsertEndChild(pEasyNode);
	tinyxml2::XMLNode * pNormalNode = xml_doc.NewElement("normal");
	pRoot->InsertEndChild(pNormalNode);
	tinyxml2::XMLNode * pHardNode = xml_doc.NewElement("hard");
	pRoot->InsertEndChild(pHardNode);
	tinyxml2::XMLNode * pGeneralNode = xml_doc.NewElement("general");
	pRoot->InsertEndChild(pGeneralNode);

	//Insert save data
	tinyxml2::XMLElement * pEasyNameElement = xml_doc.NewElement("name");
	pEasyNameElement->SetText(name[0].c_str());
	pEasyNode->InsertEndChild(pEasyNameElement);
	tinyxml2::XMLElement * pEasyScoreElement = xml_doc.NewElement("score");
	pEasyScoreElement->SetText(score[0]);
	pEasyNode->InsertEndChild(pEasyScoreElement);
	tinyxml2::XMLElement * pEasyItemsDestroyedElement = xml_doc.NewElement("itemsDestroyed");
	pEasyItemsDestroyedElement->SetText(itemsDestroyed[0]);
	pEasyNode->InsertEndChild(pEasyItemsDestroyedElement);

	tinyxml2::XMLElement * pNormalNameElement = xml_doc.NewElement("name");
	pNormalNameElement->SetText(name[1].c_str());
	pNormalNode->InsertEndChild(pNormalNameElement);
	tinyxml2::XMLElement * pNormalScoreElement = xml_doc.NewElement("score");
	pNormalScoreElement->SetText(score[1]);
	pNormalNode->InsertEndChild(pNormalScoreElement);
	tinyxml2::XMLElement * pNormalItemsDestroyedElement = xml_doc.NewElement("itemsDestroyed");
	pNormalItemsDestroyedElement->SetText(itemsDestroyed[1]);
	pNormalNode->InsertEndChild(pNormalItemsDestroyedElement);

	tinyxml2::XMLElement * pHardNameElement = xml_doc.NewElement("name");
	pHardNameElement->SetText(name[2].c_str());
	pHardNode->InsertEndChild(pHardNameElement);
	tinyxml2::XMLElement * pHardScoreElement = xml_doc.NewElement("score");
	pHardScoreElement->SetText(score[2]);
	pHardNode->InsertEndChild(pHardScoreElement);
	tinyxml2::XMLElement * pHardItemsDestroyedElement = xml_doc.NewElement("itemsDestroyed");
	pHardItemsDestroyedElement->SetText(itemsDestroyed[2]);
	pHardNode->InsertEndChild(pHardItemsDestroyedElement);

	tinyxml2::XMLElement * pGeneralSkinElement = xml_doc.NewElement("skinID");
	pGeneralSkinElement->SetText(skin);
	pGeneralNode->InsertEndChild(pGeneralSkinElement);

	//Actually save
	xml_doc.SaveFile(fileLocation.c_str());
}

int AoTSJSaveManager::getScore(int dif){
	return score[dif];
}

int AoTSJSaveManager::getItemsDestroyed(int dif){
	return itemsDestroyed[dif];
}

int AoTSJSaveManager::getPlayerSkin(){
	return skin;
}

string AoTSJSaveManager::getName(int dif){
	return name[dif];
}

void AoTSJSaveManager::setScore(int dif, int scr){
	score[dif] = scr;
}

void AoTSJSaveManager::setItemsDestroyed(int dif, int idstry){
	itemsDestroyed[dif] = idstry;
}

void AoTSJSaveManager::setPlayerSkin(int skinID){
	skin=skinID;
}