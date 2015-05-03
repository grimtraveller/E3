
#pragma once

#include <algorithm>
#include <cstdint>

#include <e3_Buffer.h>



namespace e3 {

    class AudioBuffer : public Buffer < float >
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

        int64_t getNumFrames() const           { return numFrames_; }
        int64_t calcNumBytes() const           { return size_ * sizeof(float); }
        int64_t calcNumBytes(int64_t numFrames) const;

        bool hasData() const                   { return size_ == numFrames_ * numChannels_ && size_ > 0; }
        int64_t getPos() const                 { return framePos_; }
        int64_t getAvailable() const           { return numFrames_ - framePos_; }
        int64_t getAvailable(int64_t numFrames) const;
        bool isAvailable(int64_t numFrames) const;
        bool hasAvailable() const            { return framePos_ < numFrames_; }
        bool isHead() const                  { return framePos_ == 0; }
        bool isEnd() const                   { return framePos_ == numFrames_; }

        void seek(int64_t framePos)            { framePos_ = framePos; }
        void advance(int64_t inc)              { framePos_ += inc; }
        float* getCurrent();

        float* resize(size_t size, bool clearData = true);

    protected:
        int sampleRate_;
        int numChannels_;
        int64_t numFrames_;
        int64_t framePos_;
    };


    // AudioBuffer inlines

    inline int64_t AudioBuffer::getAvailable(int64_t numFrames) const
    {
        return std::min<int64_t>(numFrames, numFrames_ - framePos_);
    }

    inline bool AudioBuffer::isAvailable(int64_t numFrames) const
    {
        return framePos_ + numFrames <= numFrames_;
    }

    inline int64_t AudioBuffer::calcNumBytes(int64_t numFrames) const
    {
        return numFrames * numChannels_ * sizeof(float);
    }

} // namespace e3