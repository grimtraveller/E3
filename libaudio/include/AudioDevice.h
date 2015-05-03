
//-------------------------------------------------------------------
// Represents a physical audio device for realtime audio i/o
//-------------------------------------------------------------------

#pragma once

#include <string>
#include <vector>

#include <IntegerTypes.h>
#include <AudioBridge.h>


namespace e3 {

    //--------------------------------------------------------------------
    // Contains all information about an AudioDevice
    //--------------------------------------------------------------------
    class AudioDeviceInfo
    {
    public:
        typedef std::vector<unsigned> SampleRatesVector;

        AudioDeviceInfo();
        AudioDeviceInfo(const AudioDeviceInfo& other);

        AudioPortId getPortId() const                { return portId_; }
        const std::string& getPortName() const       { return AudioBridge::getPortNames().getShortName(portId_); }
        int getDeviceId() const                      { return deviceId_; }
        int getPrivateId() const                     { return privateId_; }
        const std::string& getDeviceName() const     { return name_; }
        bool isAvailable() const                     { return isAvailable_; }
        bool isDefault() const                       { return isDefault_; }
        int getMaxInputChannels() const              { return maxInputChannels_; }
        int getMaxOutputChannels() const             { return maxOutputChannels_; }
        int getDefaultSampleRate() const             { return defaultSampleRate_; }
        int64 getSupportedDataFormats() const        { return supportedDataFormats_; }
        double getDefaultLowInputLatency() const     { return defaultLowInputLatency_; }
        double getDefaultLowOutputLatency() const    { return defaultLowOutputLatency_; }
        double getDefaultHighInputLatency() const    { return defaultHighInputLatency_; }
        double getDefaultHighOutputLatency() const   { return defaultHighOutputLatency_; }

        void setPortId(AudioPortId id)               { portId_ = id; }
        void setDeviceId(int deviceId)               { deviceId_ = deviceId; }
        void setPrivateId(int privateId)             { privateId_ = privateId; }
        void setDeviceName(const std::string& name)  { name_ = name; }
        void setAvailable(bool isAvailable)          { isAvailable_ = isAvailable; }
        void setDefault(bool isDefault)              { isDefault_ = isDefault; }
        void setMaxInputChannels(int num)            { maxInputChannels_ = num; }
        void setMaxOutputChannels(int num)           { maxOutputChannels_ = num; }
        void setDefaultSampleRate(int rate)          { defaultSampleRate_ = rate; }
        void setSupportedDataFormats(int code)       { supportedDataFormats_ = code; }
        void setDefaultLowInputLatency(double val)   { defaultLowInputLatency_ = val; }
        void setDefaultLowOutputLatency(double val)  { defaultLowOutputLatency_ = val; }
        void setDefaultHighInputLatency(double val)  { defaultHighInputLatency_ = val; }
        void setDefaultHighOutputLatency(double val) { defaultHighOutputLatency_ = val; }

        virtual bool isInput() const                 { return getMaxInputChannels() > 0; }
        virtual bool isOutput() const                { return getMaxOutputChannels() > 0; }
        virtual bool isDuplex() const                { return isInput() && isOutput(); }

        virtual const SampleRatesVector& getSupportedSampleRates() const     { return supportedSampleRates_; }
        virtual void setSupportedSampleRates(const SampleRatesVector& rates) { supportedSampleRates_ = rates; }

        virtual bool isSampleRateSupported(unsigned sampleRate) const;
        virtual unsigned findBestSampleRate(unsigned sampleRate) const;

    protected:
        std::string name_;
        AudioPortId portId_;
        int deviceId_;
        int privateId_;
        bool isAvailable_;
        bool isDefault_;

        int maxInputChannels_;
        int maxOutputChannels_;

        int64 supportedDataFormats_;

        double defaultLowInputLatency_;
        double defaultLowOutputLatency_;

        double defaultHighInputLatency_;
        double defaultHighOutputLatency_;

        int defaultSampleRate_;
        mutable SampleRatesVector supportedSampleRates_;

        static const unsigned sampleRatesToTry_s[];
        static const unsigned numSampleRatesToTry_s;
    };


    //--------------------------------------------------------------------
    // A physical audio device
    //--------------------------------------------------------------------
    class AudioDevice : public AudioDeviceInfo
    {
    public:
        enum SampleFormat
        {
            SF_Int8 = paInt8,
            SF_Int16 = paInt16,
            SF_Int24 = paInt24,
            SF_Int32 = paInt32,
            SF_UInt8 = paUInt8,
            SF_Float32 = paFloat32,
            SF_Custom = paCustomFormat
        };

        enum StreamCallbackResult
        {
            CallbackContinue = 0,   // Signal that the stream should continue invoking the callback and processing audio. 
            CallbackComplete = 1,   // Signal that the stream should stop invoking the callback and finish once all output samples have played. 
            CallbackAbort = 2       // Signal that the stream should stop invoking the callback and finish as soon as possible. 
        };


    public:
        AudioDevice(const AudioDeviceInfo& deviceInfo);
        ~AudioDevice();

        void setSampleRate(int sampleRate)        { sampleRate_ = sampleRate; }
        int getSampleRate() const                 { return sampleRate_; }

        void setNumChannels(int numChannels)      { numChannels_ = numChannels; }
        int getNumChannels() const                { return numChannels_; }

        void setBufferSize(uint32_t bufferSize)   { bufferSize_ = bufferSize; }
        uint32_t getBufferSize() const            { return bufferSize_; }

        void setSampleFormat(SampleFormat format) { sampleFormat_ = format; }
        SampleFormat getSampleFormat() const      { return sampleFormat_; }

        void setInterleaved(bool value)           { interleaved_ = value; }
        bool getInterleaved() const               { return interleaved_; }

        void setHostApiSpecificStreamInfo(void* info)     { hostApiSpecificStreamInfo_ = info; }

        //static int paStreamCallback(
        //    const void* input, 
        //    void* output,
        //    unsigned long numFrames,
        //    const PaStreamCallbackTimeInfo* timeInfo,
        //    PaStreamCallbackFlags status,
        //    void* userData);

        typedef PaStreamCallbackTimeInfo StreamCallbackTimeInfo;
        typedef PaStreamCallbackFlags StreamCallbackFlags;
        //typedef PaStreamCallback StreamCallback;

        typedef int StreamCallback(
            const void* input,
            void* output,
            unsigned long numFrames,
            const StreamCallbackTimeInfo* timeInfo,
            StreamCallbackFlags status,
            void* userData);

        StreamCallback* streamCallback_;
        void* callbackData_;

        void openCallbackStream(StreamCallback* callback, void* userData);
        void startCallbackStream();
        void closeCallbackStream();
        bool isStreamActive() const;

        double getStreamLatency() const;           //! The latency of an opened stream in seconds
        int getStreamSampleRate() const;           //! The sample rate of an opened stream
        double getStreamCPULoad() const;

        bool isSampleRateSupported(unsigned sampleRate) const;

    protected:
        PaStream* stream_;

        int sampleRate_;
        int numChannels_;
        unsigned bufferSize_;
        SampleFormat sampleFormat_;
        bool interleaved_;
        void* hostApiSpecificStreamInfo_;
    };

} // namespace e3

