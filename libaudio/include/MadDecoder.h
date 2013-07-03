
//------------------------------------------------------------
// MadDecoder.h
// Wrapper class for libmad
//------------------------------------------------------------

#pragma once


#include <stdio.h>

#include <mad.h>

#include <IntegerTypes.h>
#include <AudioBuffer.h>
#include <AudioFormat.h>



class MadDecoder
{
public:
    MadDecoder();

    size_t start(FILE* handle);
    void finish();
    size_t decode(size_t len, AudioBuffer* buffer);

    int getSampleRate() const   { return madSynth_.pcm.samplerate; }
    int getNumChannels() const;

    CodecId getCodecId() const;
    static const char* getVersionString();

protected:
    int64 getDurationMs(unsigned char* buffer, size_t bufferSize);
    bool readMpgFile();
    bool consumeId3Tag();

    typedef Buffer<unsigned char> CharBuffer;
    CharBuffer decodeBuffer_;
    
    FILE* handle_;
    size_t bufferSize_;
    int currentFrame_;
    int numMpegFrames_;
    int64 durationMsec_;
    bool initialized_;

    struct mad_stream madStream_;
    struct mad_frame  madFrame_;
    struct mad_synth  madSynth_;
    mad_timer_t       madTimer_;

    static unsigned long xingFrames(struct mad_bitptr ptr, unsigned bitlen);
    static void timerMultiply(mad_timer_t* t, double d);
    static int getId3TagSize(const unsigned char* data, size_t length);
};

