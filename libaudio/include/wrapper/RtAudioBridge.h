
#pragma once

#include <string>
#include <RtAudio.h>

#include <core/AudioBridge.h>
#include <core/AudioDevice.h>
#include <core/CPUProfiler.h>



//---------------------------------------------------------
// class RtAudioBridge
//---------------------------------------------------------

class RtAudioBridge : public AudioBridge
{
friend class RtAudioDevice;

public:
    RtAudioBridge();

    AudioDevicePtr getDevice(int deviceId) const;
    
protected:
    typedef EnumMapper<AudioPortId, RtAudio::Api> PortIdMapper;
    static PortIdMapper portIdMapper_s;

private:
    struct Initializer { Initializer(); };
    static Initializer initializer_s;
};



//---------------------------------------------------
// class RtAudioDevice
//---------------------------------------------------

class RtAudioDevice : public AudioDevice
{
public:
    RtAudioDevice(const AudioDeviceInfo& deviceInfo);
    ~RtAudioDevice();

    void openCallbackStream(StreamCallback* callback, void* userData);
    void startCallbackStream();
    void closeCallbackStream();
    bool isStreamActive() const;

    double getStreamLatency() const;
    int getStreamSampleRate() const;
    double getStreamCPULoad() const { return profiler_.getAverageLoad(); }

    static int rtAudioCallback(
        void* output, 
        void* input, 
        unsigned int numFrames, 
        double streamTime, 
        RtAudioStreamStatus status, 
        void* userData);

    void setSampleRate(int sampleRate);

protected:
    typedef EnumMapper<SampleFormat, RtAudioFormat> SampleFormatMapper;
    static SampleFormatMapper sampleFormatMapper_s;
    
private:
    RtAudio* rtAudio_;
    CPUProfiler profiler_;

    struct Initializer { Initializer(); };
    static Initializer initializer_s;
};
