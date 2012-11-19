
#include <Macros.h>

#include <wrapper/PortAudioBridge.h>


//---------------------------------------------------------
// class PortAudioBridge
//---------------------------------------------------------

PortAudioBridge::PortIdMapper PortAudioBridge::portIdMapper_s;
PortAudioBridge::Initializer PortAudioBridge::initializer_s;


PortAudioBridge::Initializer::Initializer()
{
    portIdMapper_s.add( AP_UNSPECIFIED,  paInDevelopment );
    portIdMapper_s.add( AP_ASIO,         paASIO );
    portIdMapper_s.add( AP_DS,           paDirectSound );
    portIdMapper_s.add( AP_MME,          paMME );
    portIdMapper_s.add( AP_WASAPI,       paWASAPI );
    portIdMapper_s.add( AP_WDMKS,        paWDMKS );
    portIdMapper_s.add( AP_ALSA,         paALSA );
    portIdMapper_s.add( AP_OSS,          paOSS );
    portIdMapper_s.add( AP_JACK,         paJACK );
    portIdMapper_s.add( AP_COREAUDIO,    paCoreAudio );
    portIdMapper_s.add( AP_SOUNDMANAGER, paSoundManager );
    portIdMapper_s.add( AP_BEOS,         paBeOS );
    portIdMapper_s.add( AP_HPI,          paAudioScienceHPI );
}



PortAudioBridge::PortAudioBridge() : AudioBridge()
{
    PaError error = Pa_Initialize();
	if(error != paNoError) 
		EXCEPTION(std::exception, Pa_GetErrorText(error));

    //PaHostApiIndex paDefaultPort  = Pa_GetDefaultHostApi();
    PaDeviceIndex paDefaultInDevice  = Pa_GetDefaultInputDevice(); 
    PaDeviceIndex paDefaultOutDevice = Pa_GetDefaultOutputDevice(); 

    for(int idxPort=0; idxPort<Pa_GetHostApiCount(); idxPort++)
    {
        const PaHostApiInfo* paInfo = Pa_GetHostApiInfo(idxPort);

        if(paInfo != NULL) 
        {
            AudioPortId portId = portIdMapper_s.getLeft(paInfo->type);
            int paApiIndex     = Pa_HostApiTypeIdToHostApiIndex(paInfo->type);

            //if(idxPort == paDefaultPort) 
            //    defaultPort_ = portId;

            for(int idxDevice=0; idxDevice<paInfo->deviceCount; idxDevice++) 
            {
                int paDeviceId = Pa_HostApiDeviceIndexToDeviceIndex(paApiIndex, idxDevice);
                
                const PaDeviceInfo* paInfo = Pa_GetDeviceInfo(paDeviceId);
                if(paInfo == NULL) 
                    continue;

                AudioDeviceInfo deviceInfo;
                deviceInfo.setDeviceName                (paInfo->name);
                deviceInfo.setPortId                    (portId);
                deviceInfo.setDeviceId                  (paDeviceId);
                deviceInfo.setPrivateId                 (paDeviceId);
                deviceInfo.setAvailable                 (true);
                deviceInfo.setDefault                   (paDeviceId == paDefaultInDevice || paDeviceId == paDefaultOutDevice);
                deviceInfo.setMaxInputChannels          (paInfo->maxInputChannels);
                deviceInfo.setMaxOutputChannels         (paInfo->maxOutputChannels);
                deviceInfo.setDefaultSampleRate         ((int)paInfo->defaultSampleRate);
                deviceInfo.setDefaultLowInputLatency    (paInfo->defaultLowInputLatency);
                deviceInfo.setDefaultLowOutputLatency   (paInfo->defaultLowOutputLatency);
                deviceInfo.setDefaultHighInputLatency   (paInfo->defaultHighInputLatency);
                deviceInfo.setDefaultHighOutputLatency  (paInfo->defaultHighOutputLatency);

                findSupportedSampleRates(deviceInfo);
                devices_.push_back(deviceInfo);
            }
        }
    }
}



PortAudioBridge::~PortAudioBridge()
{
    PaError error = Pa_Terminate();
	if(error != paNoError) 
        EXCEPTION(std::exception, Pa_GetErrorText(error));
}



AudioDevicePtr PortAudioBridge::getDevice(int deviceId) const
{
    const AudioDeviceInfo& deviceInfo = getDeviceInfo(deviceId);
    return AudioDevicePtr(new PortAudioDevice(deviceInfo));
}



void PortAudioBridge::findSupportedSampleRates(AudioDeviceInfo& deviceInfo) const
{
    const static unsigned sampleRatesToTry[]  = { 8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000, 88200, 96000, 192000 };
    AudioDeviceInfo::SampleRatesVector rates;

    PaStreamParameters p;

    p.device                    = deviceInfo.getDeviceId();
    p.channelCount              = 1;
    p.sampleFormat              = paFloat32;
    p.suggestedLatency          = deviceInfo.getDefaultHighOutputLatency();
    p.hostApiSpecificStreamInfo = NULL;

    for(unsigned i=0; i<ARRAY_LENGTH(sampleRatesToTry); i++)             // lazy initialization for sample rates
    {
        unsigned sampleRate = sampleRatesToTry[i];

        int result = deviceInfo.isInput() ? 
            Pa_IsFormatSupported(&p, NULL, sampleRate) :
            Pa_IsFormatSupported(NULL, &p, sampleRate);

        if(result == paNoError)                   // let overrides actually test it
            rates.push_back(sampleRate);
    }
    deviceInfo.setSupportedSampleRates(rates);
}




//---------------------------------------------------------
// class PortAudioDevice
//---------------------------------------------------------

PortAudioDevice::SampleFormatMapper PortAudioDevice::sampleFormatMapper_s;
PortAudioDevice::Initializer PortAudioDevice::initializer_s;


PortAudioDevice::Initializer::Initializer()
{
    sampleFormatMapper_s.add( SF_Int8,    paInt8  );
    sampleFormatMapper_s.add( SF_Int16,   paInt16 );
    sampleFormatMapper_s.add( SF_Int24,   paInt24 );
    sampleFormatMapper_s.add( SF_Int32,   paInt32 );
    sampleFormatMapper_s.add( SF_UInt8,   paUInt8 );
    sampleFormatMapper_s.add( SF_Float32, paFloat32 );
    sampleFormatMapper_s.add( SF_Custom,  paCustomFormat );
}


PortAudioDevice::PortAudioDevice(const AudioDeviceInfo& deviceInfo) : 
    AudioDevice(deviceInfo),
    stream_(NULL)
{}



bool PortAudioDevice::isSampleRateSupported(unsigned sampleRate) const
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



void PortAudioDevice::openCallbackStream(StreamCallback* callback, void* userData)
{
    PaStreamParameters params;
    memset(&params, 0, sizeof(params)); 

    params.channelCount              = numChannels_;
    params.device                    = privateId_;
    params.suggestedLatency          = getDefaultLowOutputLatency();
    params.hostApiSpecificStreamInfo = hostApiSpecificStreamInfo_; 
    params.sampleFormat              = sampleFormatMapper_s.getRight(sampleFormat_);

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
        &paStreamCallback, 
        this); 
            
	if(error != paNoError) 
        EXCEPTION(std::exception, Pa_GetErrorText(error));

    streamCallback_ = callback;
    callbackData_   = userData;
}



int PortAudioDevice::paStreamCallback(
    const void* input, 
    void* output,
    unsigned long numFrames,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags status,
    void* userData)
{
    AudioDevice* device = (AudioDevice*)userData;
    return device->streamCallback_(input, output, numFrames, (void*)timeInfo, status, device->callbackData_);
}



void PortAudioDevice::startCallbackStream()
{
    Pa_Sleep(1);

    PaError error = Pa_StartStream(stream_);
    ASSERT(error == paNoError);
}



void PortAudioDevice::closeCallbackStream()
{
    if(isStreamActive() == false) {
        return;
    }
    PaError error = Pa_CloseStream(stream_);
    ASSERT(error == paNoError);
}



bool PortAudioDevice::isStreamActive() const
{
    if(stream_ == NULL)
        return false;

    PaError result = Pa_IsStreamActive(stream_);
    ASSERT(result >= 0);

    return result == 1;
}



double PortAudioDevice::getStreamLatency() const
{
    const PaStreamInfo* info = Pa_GetStreamInfo(stream_);
    if(info == NULL)
        return 0;

    double latency = info->inputLatency + info->outputLatency;
    return latency;
}



int PortAudioDevice::getStreamSampleRate() const
{
    const PaStreamInfo* info = Pa_GetStreamInfo(stream_);
    if(info == NULL)
        return 0;

    return (int)info->sampleRate;
}



 double PortAudioDevice::getStreamCPULoad() const
 {
     return Pa_GetStreamCpuLoad(stream_);
 }











