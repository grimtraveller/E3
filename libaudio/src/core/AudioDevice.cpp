
#include <algorithm>

#include <IntegerTypes.h>
#include <Macros.h>

#include <core/AudioDevice.h>


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



