
//------------------------------------------------------------
// MadDecoder.h
// Wrapper class for libmad
//------------------------------------------------------------

#pragma once


#include <stdio.h>

#include <mad.h>

#include <IntegerTypes.h>
#include <AudioBuffer.h>



class MadDecoder
{
public:
    MadDecoder();

    size_t start(FILE* handle, size_t bufferSize);
    void finish();
    size_t decode(size_t len, AudioBuffer* buffer);

    int getSampleRate() const   { return madSynth_.pcm.samplerate; }
    //int64 getNumFrames() const  { return (int64)(durationMsec_ * .001 * (int64)getSampleRate() + .5); }
    int getNumChannels() const;

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

    struct mad_stream madStream_;
    struct mad_frame  madFrame_;
    struct mad_synth  madSynth_;
    mad_timer_t       madTimer_;

    static unsigned long xingFrames(struct mad_bitptr ptr, unsigned bitlen);
    static void timerMultiply(mad_timer_t* t, double d);
    static int getId3TagSize(const unsigned char* data, size_t length);
};

