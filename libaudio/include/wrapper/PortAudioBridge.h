
#pragma once

#include <string>
#include <portaudio.h>

#include <core/AudioBridge.h>
#include <core/AudioDevice.h>



//---------------------------------------------------------
// class PortAudioBridge
//---------------------------------------------------------

class PortAudioBridge : public AudioBridge
{
friend class PortAudioDevice;

public:
    PortAudioBridge();
    ~PortAudioBridge();

    AudioDevicePtr getDevice(int deviceId) const;
    
protected:
    typedef EnumMapper<AudioPortId, PaHostApiTypeId> PortIdMapper;
    static PortIdMapper portIdMapper_s;

private:
    void findSupportedSampleRates(AudioDeviceInfo& deviceInfo) const;

    struct Initializer { Initializer(); };
    static Initializer initializer_s;
};


//---------------------------------------------------
// class PortAudioDevice
//---------------------------------------------------

class PortAudioDevice : public AudioDevice
{
public:
    PortAudioDevice(const AudioDeviceInfo& deviceInfo);

    bool isSampleRateSupported(unsigned sampleRate) const;

    void openCallbackStream(StreamCallback* callback, void* userData);
    void startCallbackStream();
    void closeCallbackStream();
    bool isStreamActive() const;

    double getStreamLatency() const;
    int getStreamSampleRate() const;
    double getStreamCPULoad() const;

    static int paStreamCallback(
        const void* input, 
        void* output,
        unsigned long numFrames,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags status,
        void* userData);

protected:
    typedef EnumMapper<SampleFormat, PaSampleFormat> SampleFormatMapper;
    static SampleFormatMapper sampleFormatMapper_s;

private:
    PaStream* stream_;

    struct Initializer { Initializer(); };
    static Initializer initializer_s;
};
