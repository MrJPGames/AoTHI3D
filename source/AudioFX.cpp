#include "AudioFX.h"

AudioFX::AudioFX(){
}

void AudioFX::setChannel(int var1, int var2){
    channel_range=var2;
    for (int i=0; i<channel_range; i++){
        ndspChnWaveBufClear(channel[i]);
        channel[i]=var1+i;
    }
}

bool AudioFX::initSfx(const char* name){
    u32 sampleRate;
    u32 dataSize;
    u16 channels;
    u16 bitsPerSample;
    FILE* fp = fopen(name, "rb");
  
    if(!fp){
        //Could not open file
        return 0;
    }
    char signature[4];
  
    fread(signature, 1, 4, fp);
  
    if( signature[0] != 'R' &&
        signature[1] != 'I' &&
        signature[2] != 'F' &&
        signature[3] != 'F'){
        //Wrong file format
        fclose(fp);
        return 0;
    }
    fseek(fp, 22, SEEK_SET);
    fread(&channels, 2, 1, fp);
    fseek(fp, 24, SEEK_SET);
    fread(&sampleRate, 4, 1, fp);
    fseek(fp, 34, SEEK_SET);
    fread(&bitsPerSample, 2, 1, fp);
    fseek(fp,46,SEEK_END);
    dataSize = ftell(fp);
  
    if(dataSize == 0 || (channels != 1 && channels != 2) ||
        (bitsPerSample != 8 && bitsPerSample != 16))
    {
        //Corrupted file
        fclose(fp);
        return 0;
    }
  
    // Allocating and reading samples
    u8* data = (u8*)linearAlloc(dataSize);
  
    if(!data)
    {
        fclose(fp);
        return 0;
    }
  
    fseek(fp, 44, SEEK_SET);
    fread(data, 1, dataSize, fp);
    fclose(fp);
  
    // Find the right format
    u16 ndspFormat;
  
    if(bitsPerSample == 8)
    {
        ndspFormat = (channels == 1) ?
            NDSP_FORMAT_MONO_PCM8 :
            NDSP_FORMAT_STEREO_PCM8;
    }
    else
    {
        ndspFormat = (channels == 1) ?
            NDSP_FORMAT_MONO_PCM16 :
            NDSP_FORMAT_STEREO_PCM16;
    }
  
    for (int i=0; i<channel_range; i++){
        ndspChnReset(channel[i]);
        ndspChnSetInterp(channel[i], NDSP_INTERP_NONE);
        ndspChnSetRate(channel[i], (float)sampleRate);
        ndspChnSetFormat(channel[i], ndspFormat);
    }
  
    // Create wav buffer
    memset(&waveBuf, 0, sizeof(waveBuf));
  
    waveBuf.data_vaddr = (const void*)data;
    waveBuf.nsamples = dataSize / (bitsPerSample >> 2);
    waveBuf.looping = false; // Loop enabled
    waveBuf.status = NDSP_WBUF_FREE;
    DSP_FlushDataCache(data, dataSize);
    return 1;
}

void AudioFX::play(){
    for (int i=0; i<channel_range; i++){
        if (!ndspChnIsPlaying(channel[i])){
            ndspChnWaveBufClear(channel[i]);
            ndspChnWaveBufAdd(channel[i], &waveBuf);
            break;
        }
    }
}

int AudioFX::fullChns(){
    int ret=0;
    for (int i=0; i<channel_range; i++){
        if (ndspChnIsPlaying(channel[i])){
            ret+=1;
        }
    }
    return ret;
}

int AudioFX::getChannelRange(){
    return channel_range;
}