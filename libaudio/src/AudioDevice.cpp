
#include <algorithm>

#include <CommonMacros.h>
#include <IntegerTypes.h>

#include <AudioDevice.h>


//------------------------------------------------------
// class AudioDeviceInfo
//------------------------------------------------------

const unsigned AudioDeviceInfo::sampleRatesToTry_s[]  = { 8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000, 88200, 96000, 192000 };
const unsigned AudioDeviceInfo::numSampleRatesToTry_s = ARRAY_LENGTH(sampleRatesToTry_s);


AudioDeviceInfo::AudioDeviceInfo() : 
    portId_(AP_UNSPECIFIED),
    deviceId_(AudioBridge::NoDevice),
    isAvailable_(false),
    isDefault_(false),
    maxInputChannels_(0),
    maxOutputChannels_(0),
    defaultSampleRate_(0),
    supportedDataFormats_(0),
    defaultLowInputLatency_(0),
    defaultLowOutputLatency_(0),
    defaultHighInputLatency_(0),
    defaultHighOutputLatency_(0)
{}



AudioDeviceInfo::AudioDeviceInfo(const AudioDeviceInfo& other)
{
    *this = other;
}



bool AudioDeviceInfo::isSampleRateSupported(unsigned sampleRate) const
{
    SampleRatesVector::const_iterator pos;
    pos = std::find(supportedSampleRates_.begin(), supportedSampleRates_.end(), sampleRate);
    bool result = (pos != supportedSampleRates_.end());

    return result;
}



unsigned AudioDeviceInfo::findBestSampleRate(unsigned sampleRate) const
{
    for(unsigned i=0; i<numSampleRatesToTry_s; i++)
    {
        int minDiff   = std::numeric_limits<int>::max(); 
        int bestIndex = -1;

        for(unsigned j=i; j<numSampleRatesToTry_s; j++)
        {
            int diff = abs((int)sampleRate - (int)sampleRatesToTry_s[j]);
            if(diff < minDiff) {
                bestIndex = j;
                minDiff   = diff;
            }
        }
        ASSERT(bestIndex > -1);

        sampleRate = sampleRatesToTry_s[bestIndex];

        if(isSampleRateSupported(sampleRate)) {
            return sampleRate;
        }
    }
    return 0;
}



//------------------------------------------------------
// class AudioDevice
//------------------------------------------------------

AudioDevice::AudioDevice(const AudioDeviceInfo& deviceInfo) : 
    AudioDeviceInfo(deviceInfo),
    stream_(NULL),
    streamCallback_(NULL),
    callbackData_(NULL),
    sampleRate_(44100),
    numChannels_(2),
    bufferSize_(512),
    sampleFormat_(SF_Float32),
    interleaved_(true),
    hostApiSpecificStreamInfo_(NULL)
{}



AudioDevice::~AudioDevice()
{
    closeCallbackStream();
}



bool AudioDevice::isSampleRateSupported(unsigned sampleRate) const
{
    if(AudioDeviceInfo::isSampleRateSupported(sampleRate))
        return true;

    PaStreamParameters p;

    p.device                    = privateId_;
    p.channelCount              = 1;
    p.sampleFormat              = paFloat32;
    p.suggestedLatency          = getDefaultHighOutputLatency();
    p.hostApiSpecificStreamInfo = NULL;

    int result = isInput() ? 
        Pa_IsFormatSupported(&p, NULL, sampleRate) :
        Pa_IsFormatSupported(NULL, &p, sampleRate);
    return (result == paNoError);
}



void AudioDevice::openCallbackStream(StreamCallback* callback, void* userData)
{
    PaStreamParameters params;
    memset(&params, 0, sizeof(params)); 

    params.channelCount              = numChannels_;
    params.device                    = privateId_;
    params.suggestedLatency          = getDefaultLowOutputLatency();
    params.hostApiSpecificStreamInfo = hostApiSpecificStreamInfo_; 
    params.sampleFormat              = sampleFormat_;

    if(interleaved_ == false)
        params.sampleFormat |= paNonInterleaved; 

    int bufferSize = (bufferSize_  ==  0) ? paFramesPerBufferUnspecified : bufferSize_;

    PaError error = Pa_OpenStream(
        &stream_,
        NULL,          
        &params,
        sampleRate_,
        bufferSize,
        paNoFlag,      
        callback, 
        userData); 
            
	if(error != paNoError) 
        EXCEPTION(std::exception, Pa_GetErrorText(error));

    streamCallback_ = callback;
    callbackData_   = userData;
}



//int AudioDevice::StreamCallback(
//    const void* input, 
//    void* output,
//    unsigned long numFrames,
//    const PaStreamCallbackTimeInfo* timeInfo,
//    PaStreamCallbackFlags status,
//    void* userData)
//int AudioDevice::streamCallback(
//        const void* input, 
//        void* output,
//        uintf32 numFrames,
//        StreamCallbackTimeInfo* timeInfo,
//        uintf32 status,
//        void* userData)
//{
//    AudioDevice* device = (AudioDevice*)userData;
//    return device->streamCallback_(input, output, numFrames, (void*)timeInfo, status, device->callbackData_);
//}



void AudioDevice::startCallbackStream()
{
    Pa_Sleep(1);

    PaError error = Pa_StartStream(stream_);
    ASSERT(error == paNoError);
}



void AudioDevice::closeCallbackStream()
{
    if(isStreamActive() == false) {
        return;
    }
    PaError error = Pa_CloseStream(stream_);
    ASSERT(error == paNoError);
}



bool AudioDevice::isStreamActive() const
{
    if(stream_ == NULL)
        return false;

    PaError result = Pa_IsStreamActive(stream_);
    ASSERT(result >= 0);

    return result == 1;
}



double AudioDevice::getStreamLatency() const
{
    const PaStreamInfo* info = Pa_GetStreamInfo(stream_);
    if(info == NULL)
        return 0;

    double latency = info->inputLatency + info->outputLatency;
    return latency;
}



int AudioDevice::getStreamSampleRate() const
{
    const PaStreamInfo* info = Pa_GetStreamInfo(stream_);
    if(info == NULL)
        return 0;

    return (int)info->sampleRate;
}



 double AudioDevice::getStreamCPULoad() const
 {
     return Pa_GetStreamCpuLoad(stream_);
 }






