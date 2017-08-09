#include "Online.h"
#include "TinyXML2.h"
#include <inttypes.h>

using namespace tinyxml2;
using namespace std;

Online::Online(){
	httpcInit(0);
}

Result Online::getLeaderboard(){
	std::ifstream t("/3ds/AoTSJSave.xml");
	std::stringstream buffer;
	buffer << t.rdbuf();

	cfguInit();
	u64 hash;
	CFGU_GenHashConsoleUnique(0x1337, &hash);

	u8 * outdata = (u8*)malloc(0x1C);
	u8 * outdata2 = (u8*)malloc(0x1C);
	CFGU_GetConfigInfoBlk2 (0x4, 0x000B0000, outdata);
	utf16_to_utf8(outdata2,(u16*)outdata,0x4);
	string countryInfo((char*)outdata2, 0x4);
	free(outdata);
	free(outdata2);

	cfguExit();

	string s = buffer.str();

    while ( s.find ("\n") != string::npos )
    {
        s.erase ( s.find ("\n"), 1 );
    }
    while ( s.find (" ") != string::npos )
    {
        s.erase ( s.find (" "), 1 );
    }

	string url = (string)"http://poc.debug-it.nl/AoTSJ/?uploadScore=1&country_id=" + countryInfo[4] + (string)"&id=" + to_string(hash) + (string)"&xml=" + s;

	getStringFromURL(url.c_str(), false);
	string ret = getStringFromURL("http://poc.debug-it.nl/AoTSJ/?getLeaderboard=1", true);
	if (ret == "Successful"){
		//loadStatsFromFile("/3ds/AoTSJonlinestats.xml");
		loaded=true;
		return 0;
	}else{
		return -1;
	}
}

string Online::getStringFromURL(const char* url, bool toPageData){
	Result ret=0;
	httpcContext context;
	char *newurl=NULL;
	u32 statuscode=0;
	u32 contentsize=0, readsize=0, size=0;
	u8 *buf, *lastbuf;

	do {
		ret = httpcOpenContext(&context, HTTPC_METHOD_GET, url, 1);

		// This disables SSL cert verification, so https:// will be usable
		ret = httpcSetSSLOpt(&context, SSLCOPT_DisableVerify);

		// Enable Keep-Alive connections (on by default, pending ctrulib merge)
		// ret = httpcSetKeepAlive(&context, HTTPC_KEEPALIVE_ENABLED);
		// printf("return from httpcSetKeepAlive: %"PRId32"\n",ret);
		// gfxFlushBuffers();

		// Set a User-Agent header so websites can identify your application
		ret = httpcAddRequestHeaderField(&context, "User-Agent", "AoTSJClient/1.0.0");

		// Tell the server we can support Keep-Alive connections.
		// This will delay connection teardown momentarily (typically 5s)
		// in case there is another request made to the same server.
		ret = httpcAddRequestHeaderField(&context, "Connection", "Keep-Alive");

		ret = httpcBeginRequest(&context);
		if(ret!=0){
			httpcCloseContext(&context);
			if(newurl!=NULL) free(newurl);
			return "Failed to connect";
		}

		ret = httpcGetResponseStatusCode(&context, &statuscode);
		if(ret!=0){
			httpcCloseContext(&context);
			if(newurl!=NULL) free(newurl);
			return "Failed to connect";
		}

		if ((statuscode >= 301 && statuscode <= 303) || (statuscode >= 307 && statuscode <= 308)) {
			if(newurl==NULL) newurl = (char*)malloc(0x1000); // One 4K page for new URL
			if (newurl==NULL){
				httpcCloseContext(&context);
				return "Failed (server error)";
			}
			ret = httpcGetResponseHeader(&context, "Location", newurl, 0x1000);
			url = newurl; // Change pointer to the url that we just learned
			httpcCloseContext(&context); // Close this context before we try the next
		}
	} while ((statuscode >= 301 && statuscode <= 303) || (statuscode >= 307 && statuscode <= 308));

	if(statuscode!=200){
		httpcCloseContext(&context);
		if(newurl!=NULL) free(newurl);
		return "Failed (server error)";
	}

	// This relies on an optional Content-Length header and may be 0
	ret=httpcGetDownloadSizeState(&context, NULL, &contentsize);
	if(ret!=0){
		httpcCloseContext(&context);
		if(newurl!=NULL) free(newurl);
		return "Failed to start download of leaderbaord data";
	}

	// Start with a single page buffer
	buf = (u8*)malloc(0x1000);
	if(buf==NULL){
		httpcCloseContext(&context);
		if(newurl!=NULL) free(newurl);
		return "Failed to create buffer";
	}

	do {
		// This download loop resizes the buffer as data is read.
		ret = httpcDownloadData(&context, buf+size, 0x1000, &readsize);
		size += readsize; 
		if (ret == (s32)HTTPC_RESULTCODE_DOWNLOADPENDING){
				lastbuf = buf; // Save the old pointer, in case realloc() fails.
				buf = (u8*)realloc(buf, size + 0x1000);
				if(buf==NULL){ 
					httpcCloseContext(&context);
					free(lastbuf);
					if(newurl!=NULL) free(newurl);
					return "Failed to download leaderboard data";
				}
			}
	} while (ret == (s32)HTTPC_RESULTCODE_DOWNLOADPENDING);	

	if(ret!=0){
		httpcCloseContext(&context);
		if(newurl!=NULL) free(newurl);
		free(buf);
		return "Failed to download leaderboard data (2)";
	}

	// Resize the buffer back down to our actual final size
	lastbuf = buf;
	buf = (u8*)realloc(buf, size);
	if(buf==NULL){ // realloc() failed.
		httpcCloseContext(&context);
		free(lastbuf);
		if(newurl!=NULL) free(newurl);
		return "Resizeing buffer failed";
	}

	if (toPageData){
		XMLDocument doc;

		doc.Parse( (char*)buf, size);
		XMLNode * root = doc.LastChild();
		if (root != nullptr){
			pageCount=0;
			pageLines=0;

			int i=0, j;
			for (XMLNode * node = root->FirstChild(); node != nullptr; node = node->NextSibling()){
				j=1;
				pages[i][0] = node->FirstChildElement("title")->GetText();
				for (XMLElement * item_element = node->FirstChildElement("item0"); item_element != nullptr; item_element = item_element->NextSiblingElement()){
					//NOTE: might have to be node->FirstChildElement("item0")!
					pages[i][j] = item_element->GetText();
					if (j > pageLines)
						pageLines=j;
					j++;
				}
				i++;
				pageCount++;
			}
		}
	}


	httpcCloseContext(&context);
	//string str((char*)buf, size);
	free(buf);
	if (newurl!=NULL) free(newurl);



	printf("End of DL function\n");
	gfxFlushBuffers();
	gfxSwapBuffers();
	return "Successful";
}

bool Online::isLoaded(){
	return loaded;
}

void Online::setLoaded(bool ld){
	loaded=ld;
}

int Online::getPageLines(){
	return pageLines;
}

int Online::getPageCount(){
	return pageCount;
}

string Online::getPageData(int page_num, int page_line){
	return pages[page_num][page_line];
}

string Online::getRawPageData(){
	return pageData;
}