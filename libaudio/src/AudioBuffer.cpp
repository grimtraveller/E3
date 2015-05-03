
#include <samplerate.h> // libsamplerate

#include <e3_CommonMacros.h>
#include <e3_Exception.h>
#include <AudioBuffer.h>


namespace e3 {

    AudioBuffer::AudioBuffer(int numChannels) :
        Buffer(),
        sampleRate_(0),
        numFrames_(0),
        numChannels_(numChannels),
        framePos_(0)
    {}


    AudioBuffer::AudioBuffer(const AudioBuffer& source) :
        Buffer(source),
        sampleRate_(source.sampleRate_),
        numFrames_(source.numChannels_),
        numChannels_(source.numChannels_),
        framePos_(0)
    {}



    AudioBuffer& AudioBuffer::operator= (const AudioBuffer& source)
    {
        copy(source);
        sampleRate_  = source.sampleRate_;
        numFrames_   = source.numFrames_;
        numChannels_ = source.numChannels_;
        framePos_    = source.numFrames_;

        return *this;
    }



    void AudioBuffer::convertSampleRate(int newRate)
    {
        if (sampleRate_ == 0) {
            setSampleRate(newRate);
        }
        else if (newRate != sampleRate_)
        {
            double ratio = (1.0 * newRate) / sampleRate_;
            if (src_is_valid_ratio(ratio) == 0) {
                THROW(std::exception, "Samplerate can not be converted from %d to %d", sampleRate_, newRate);
            }

            AudioBuffer output(numChannels_);
            output.resize((int)ceil(size_ * ratio));  // set buffer size needed for new samplerate

            // convert using libsamplerate
            SRC_DATA srcData;
            srcData.end_of_input = 0;
            srcData.input_frames = (long)getNumFrames();
            srcData.output_frames = (long)output.getNumFrames();
            srcData.data_in = getHead();
            srcData.data_out = output.getHead();
            srcData.src_ratio = ratio;

            int error = src_simple(&srcData, SRC_SINC_BEST_QUALITY, numChannels_);
            if (error != 0) {
                THROW(std::exception, src_strerror(error));
            }

            resize(output.size());
            memcpy(getHead(), output.getHead(), (size_t)calcNumBytes());

            sampleRate_ = newRate;
        }
    }



    float* AudioBuffer::resize(size_t size, bool clearData)
    {
        if (clearData)
            clear();

        float* pResult = Buffer::resize(size);

        numFrames_ = (numChannels_ > 0 && data_) ? size / numChannels_ : 0;
        return pResult;
    }


    float* AudioBuffer::getCurrent()
    {
        ASSERT(framePos_ <= numFrames_);
        return data_ + framePos_ * numChannels_;
    }

} // namespace e3

