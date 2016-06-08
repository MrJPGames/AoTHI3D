#ifndef AudioFX_H
#define AudioFX_H

#include <3ds.h>
#include <stdio.h>
#include <cstring>

class AudioFX{
	public:
		AudioFX();
		void setChannel(int var1, int var2);
		bool initSfx(const char* name);
		void play();
		int fullChns();
		int getChannelRange();
	private:
		int channel[5];
		int channel_range=5;
		ndspWaveBuf waveBuf;
};

#endif