
//-------------------------------------------------------------------
// Represents a physical audio device for realtime audio i/o
//-------------------------------------------------------------------

#pragma once

#include <string>
#include <vector>

#include <IntegerTypes.h>
#include <core/AudioBridge.h>



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
    void setDefaultLowInputLatency(double val)   { defaultLowInputLatency_   = val; }
    void setDefaultLowOutputLatency(double val)  { defaultLowOutputLatency_  = val; }
    void setDefaultHighInputLatency(double val)  { defaultHighInputLatency_  = val; }
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



class AudioDevice : public AudioDeviceInfo
{
public:
    enum SampleFormat
    {
        SF_Int8    = 1,
        SF_Int16   = 2,
        SF_Int24   = 3,
        SF_Int32   = 4,
        SF_UInt8   = 5,
        SF_Float32 = 6,
        SF_Float64 = 7,
        SF_Custom  = 8,
    };

    enum StreamCallbackResult
    {
        CallbackContinue=0,   // Signal that the stream should continue invoking the callback and processing audio. 
        CallbackComplete=1,   // Signal that the stream should stop invoking the callback and finish once all output samples have played. 
        CallbackAbort=2       // Signal that the stream should stop invoking the callback and finish as soon as possible. 
    };

    
public:
    AudioDevice(const AudioDeviceInfo& deviceInfo);
    virtual ~AudioDevice();

    virtual void setSampleRate(int sampleRate)        { sampleRate_ = sampleRate; }
    virtual int getSampleRate() const                 { return sampleRate_; }

    virtual void setNumChannels(int numChannels)      { numChannels_ = numChannels; }
    virtual int getNumChannels() const                { return numChannels_; }

    virtual void setBufferSize(uint32 bufferSize)     { bufferSize_ = bufferSize; }
    virtual uint32 getBufferSize() const              { return bufferSize_; }
    
    virtual void setSampleFormat(SampleFormat format) { sampleFormat_ = format; }
    virtual SampleFormat getSampleFormat() const      { return sampleFormat_; }

    virtual void setInterleaved(bool value)           { interleaved_ = value; }
    virtual bool getInterleaved() const               { return interleaved_; }

    void setHostApiSpecificStreamInfo(void* info)   { hostApiSpecificStreamInfo_ = info; }

    typedef int StreamCallback(
        const void* input, 
        void* output,
        uintf32 numFrames,
        void* timeInfo,
        uintf32 status,
        void* userData);
    
    StreamCallback* streamCallback_;
    void* callbackData_;

    virtual void openCallbackStream(StreamCallback* callback, void* userData) = 0;
    virtual void startCallbackStream() = 0;
    virtual void closeCallbackStream() {};
    virtual bool isStreamActive() const = 0;

    //virtual double getStreamTime() const = 0;
    virtual double getStreamLatency() const = 0;           //! The latency of an opened stream in seconds
    virtual int getStreamSampleRate() const = 0;           //! The sample rate of an opened stream
    virtual double getStreamCPULoad() const = 0;

protected:
    int sampleRate_;
    int numChannels_;
    unsigned bufferSize_;
    SampleFormat sampleFormat_;
    bool interleaved_;
    void* hostApiSpecificStreamInfo_;
};



