#include "SaveManager.h"
#include "sha256.h"

using namespace std;

inline bool AoTSJSaveManager::file_exists(const std::string& name) {
	ifstream f(name.c_str());
	return f.good();
}

string AoTSJSaveManager::getHash(){
	//Calculate hash
	//Please DO NOT cheat! (Yes it can be broken, yes the key is in the release binary, no you shouldn't go looking for it)
	string secret="private"; //Not actual key on GitHub
	string to_hash=secret+to_string(score[0])+to_string(score[1])+to_string(score[2])+to_string(itemsDestroyed[0])+to_string(itemsDestroyed[1])+to_string(itemsDestroyed[2]);
	string hash = sha256(to_hash);
	return hash;
}

void AoTSJSaveManager::loadSaveDataFromFile(string fileLocation){
	//Load local username!
	cfguInit();
	u8 * outdata = (u8*)malloc(0x1C);
	u8 * outdata2 = (u8*)malloc(0x1C);
	CFGU_GetConfigInfoBlk2 (0x1C, 0x000A0000, outdata);
	utf16_to_utf8(outdata2,(u16*)outdata,0x1C);
	string str((char*)outdata2, 0x1C);
	localUsername = str;
	free(outdata);
	free(outdata2);
	cfguExit();

	if (file_exists(fileLocation)){
		string expectedHash;

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
					element->FirstChildElement("score")->QueryIntText(&score[0]);
					element->FirstChildElement("itemsDestroyed")->QueryIntText(&itemsDestroyed[0]);
				}
				element = root->FirstChildElement("normal");
				if (element != nullptr){
					element->FirstChildElement("score")->QueryIntText(&score[1]);
					element->FirstChildElement("itemsDestroyed")->QueryIntText(&itemsDestroyed[1]);
				}
				element = root->FirstChildElement("hard");
				if (element != nullptr){
					element->FirstChildElement("score")->QueryIntText(&score[2]);
					element->FirstChildElement("itemsDestroyed")->QueryIntText(&itemsDestroyed[2]);
				}
				element = root->FirstChildElement("general");
				if (element != nullptr){
					element->FirstChildElement("skinID")->QueryIntText(&skin);
					expectedHash = element->FirstChildElement("hash")->GetText();
				}
			}
		}
		if (expectedHash != getHash()){
			for (int i=0; i<3; i++){
				score[i]=0;
				itemsDestroyed[i]=0;
			}
		}
	}else{
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
	tinyxml2::XMLElement * pEasyScoreElement = xml_doc.NewElement("score");
	pEasyScoreElement->SetText(score[0]);
	pEasyNode->InsertEndChild(pEasyScoreElement);
	tinyxml2::XMLElement * pEasyItemsDestroyedElement = xml_doc.NewElement("itemsDestroyed");
	pEasyItemsDestroyedElement->SetText(itemsDestroyed[0]);
	pEasyNode->InsertEndChild(pEasyItemsDestroyedElement);

	tinyxml2::XMLElement * pNormalScoreElement = xml_doc.NewElement("score");
	pNormalScoreElement->SetText(score[1]);
	pNormalNode->InsertEndChild(pNormalScoreElement);
	tinyxml2::XMLElement * pNormalItemsDestroyedElement = xml_doc.NewElement("itemsDestroyed");
	pNormalItemsDestroyedElement->SetText(itemsDestroyed[1]);
	pNormalNode->InsertEndChild(pNormalItemsDestroyedElement);

	tinyxml2::XMLElement * pHardScoreElement = xml_doc.NewElement("score");
	pHardScoreElement->SetText(score[2]);
	pHardNode->InsertEndChild(pHardScoreElement);
	tinyxml2::XMLElement * pHardItemsDestroyedElement = xml_doc.NewElement("itemsDestroyed");
	pHardItemsDestroyedElement->SetText(itemsDestroyed[2]);
	pHardNode->InsertEndChild(pHardItemsDestroyedElement);

	//General part
	tinyxml2::XMLElement * pGeneralNameElement = xml_doc.NewElement("name");
	pGeneralNameElement->SetText(localUsername.c_str());
	pGeneralNode->InsertEndChild(pGeneralNameElement);
	tinyxml2::XMLElement * pGeneralSkinElement = xml_doc.NewElement("skinID");
	pGeneralSkinElement->SetText(skin);
	pGeneralNode->InsertEndChild(pGeneralSkinElement);
	
	string hash = getHash();

	tinyxml2::XMLElement * pGeneralHashlement = xml_doc.NewElement("hash");
	pGeneralHashlement->SetText(hash.c_str());
	pGeneralNode->InsertEndChild(pGeneralHashlement);

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

string AoTSJSaveManager::getName(){
	return localUsername;
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