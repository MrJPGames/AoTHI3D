#ifndef Song_H
#define Song_H

#include <3ds.h>
#include <stdio.h>
#include <cstring>

class Song{
	public:
		Song();
		void setChannel(int var1);
		bool initSong(const char* name);
		void play();
		int getChannel();
	private:
		int channel=1;
		ndspWaveBuf waveBuf;
};

#endif