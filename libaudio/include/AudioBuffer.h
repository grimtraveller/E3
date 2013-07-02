
#pragma once

#include <algorithm>

#include <IntegerTypes.h>
#include <Buffer.h>


class AudioBuffer : public Buffer<float>
{
public:
    AudioBuffer(int numChannels = 0);
    AudioBuffer(const AudioBuffer& source);

    AudioBuffer& operator= (const AudioBuffer& source);

    int getSampleRate() const            { return sampleRate_; }
    int getNumChannels() const           { return numChannels_; }

    void setSampleRate(int sampleRate)   { sampleRate_ = sampleRate; }
    void setNumChannels(int numChannels) { numChannels_ = numChannels; }

    void convertSampleRate(int newRate);

    int64 getNumFrames() const           { return numFrames_; }
    int64 calcNumBytes() const           { return size_ * sizeof(float); }
    int64 calcNumBytes(int64 numFrames) const;

    bool hasData() const                 { return size_ == numFrames_ * numChannels_ && size_ > 0; }
    int64 getPos() const                 { return framePos_; }
    int64 getAvailable() const           { return numFrames_ - framePos_; }
    int64 getAvailable(int64 numFrames) const;
    bool isAvailable(int64 numFrames) const;
    bool hasAvailable() const            { return framePos_ < numFrames_; }
    bool isHead() const                  { return framePos_ == 0; }
    bool isEnd() const                   { return framePos_ == numFrames_; }

    void seek(int64 framePos)            { framePos_ = framePos; }
    void advance(int64 inc)              { framePos_ += inc; }
    float* getCurrent();

    float* resize(size_t size, bool clearData=true);

protected:
    int sampleRate_;
    int numChannels_;
    int64 numFrames_;
    int64 framePos_;
};


// AudioBuffer inlines

inline int64 AudioBuffer::getAvailable(int64 numFrames) const     
{ 
    return std::min<int64>(numFrames, numFrames_ - framePos_); 
}

inline bool AudioBuffer::isAvailable(int64 numFrames) const  
{
    return framePos_ + numFrames <= numFrames_;
}

inline int64 AudioBuffer::calcNumBytes(int64 numFrames) const    
{ 
    return numFrames * numChannels_ * sizeof(float); 
}

inline float* AudioBuffer::getCurrent()                    
{ 
    ASSERT(framePos_ <= numFrames_); 
    return data_ + framePos_ * numChannels_; 
}

