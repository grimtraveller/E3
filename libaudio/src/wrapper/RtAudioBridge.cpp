
#include <exception>
#include <boost/foreach.hpp>

#include <RtAudio.h>

#include <Macros.h>
#include <wrapper/RtAudioBridge.h>


RtAudioBridge::PortIdMapper RtAudioBridge::portIdMapper_s;
RtAudioBridge::Initializer RtAudioBridge::initializer_s;


RtAudioBridge::Initializer::Initializer()
{
    // not supported: API_MME, API_WASAPI, API_WDMKS, API_SOUNDMANAGER, API_BEOS, API_HPI
    portIdMapper_s.add( AP_UNSPECIFIED,   RtAudio::UNSPECIFIED);
    portIdMapper_s.add( AP_ASIO,          RtAudio::WINDOWS_ASIO);
    portIdMapper_s.add( AP_DS,            RtAudio::WINDOWS_DS);
    portIdMapper_s.add( AP_ALSA,          RtAudio::LINUX_ALSA);
    portIdMapper_s.add( AP_PULSE,         RtAudio::LINUX_PULSE);
    portIdMapper_s.add( AP_OSS,           RtAudio::LINUX_OSS);
    portIdMapper_s.add( AP_JACK,          RtAudio::UNIX_JACK);
    portIdMapper_s.add( AP_COREAUDIO,     RtAudio::MACOSX_CORE);
}



RtAudioBridge::RtAudioBridge() : AudioBridge()
{
    std::vector<RtAudio::Api> compiledApis;
    RtAudio::getCompiledApi(compiledApis);

    int deviceId = 0;
    AudioPortId defaultOutPort = AP_UNSPECIFIED;
    AudioPortId defaultInPort  = AP_UNSPECIFIED;

    for(unsigned idxPort=0; idxPort<compiledApis.size(); idxPort++)
    {
        try {
            RtAudio::Api api = compiledApis[idxPort];
            RtAudio rtAudio(api);
            rtAudio.showWarnings(false);

            AudioPortId portId = portIdMapper_s.getLeft(api);

            for(unsigned idxDevice=0; idxDevice<rtAudio.getDeviceCount(); idxDevice++) 
            {
                RtAudio::DeviceInfo rtInfo = rtAudio.getDeviceInfo( idxDevice );
                deviceId++;
                
                AudioDeviceInfo deviceInfo;

                deviceInfo.setDeviceName                (rtInfo.name);
                deviceInfo.setPortId                    (portId);
                deviceInfo.setDeviceId                  (deviceId);
                deviceInfo.setPrivateId                 (idxDevice);
                deviceInfo.setAvailable                 (rtInfo.probed);
                deviceInfo.setMaxInputChannels          (rtInfo.inputChannels);
                deviceInfo.setMaxOutputChannels         (rtInfo.outputChannels);
                deviceInfo.setDefaultSampleRate         (0);
                deviceInfo.setSupportedSampleRates      (rtInfo.sampleRates);
                deviceInfo.setSupportedDataFormats      (rtInfo.nativeFormats);
                deviceInfo.setDefaultLowInputLatency    (0);
                deviceInfo.setDefaultLowOutputLatency   (0);
                deviceInfo.setDefaultHighInputLatency   (0);
                deviceInfo.setDefaultHighOutputLatency  (0);

                if(defaultInPort == AP_UNSPECIFIED && rtInfo.isDefaultInput) 
                {
                    defaultInPort = portId;
                    deviceInfo.setDefault(true);   
                }
                if(defaultOutPort == AP_UNSPECIFIED && rtInfo.isDefaultOutput) 
                {
                    defaultOutPort = portId;
                    deviceInfo.setDefault(true);   
                }
                devices_.push_back(deviceInfo);
            }
        }
        catch(const RtError& e) {
            EXCEPTION(std::exception, e.what());
        }
    }
}



AudioDevicePtr RtAudioBridge::getDevice(int deviceId) const
{
    const AudioDeviceInfo& deviceInfo = getDeviceInfo(deviceId);
    return AudioDevicePtr(new RtAudioDevice(deviceInfo));
}


//---------------------------------------------------------
// class RtAudioDevice
//---------------------------------------------------------

RtAudioDevice::SampleFormatMapper RtAudioDevice::sampleFormatMapper_s;
RtAudioDevice::Initializer RtAudioDevice::initializer_s;

RtAudioDevice::Initializer::Initializer()
{
    sampleFormatMapper_s.add( SF_Int8,    RTAUDIO_SINT8 );
    sampleFormatMapper_s.add( SF_Int16,   RTAUDIO_SINT16 );
    sampleFormatMapper_s.add( SF_Int24,   RTAUDIO_SINT24 );
    sampleFormatMapper_s.add( SF_Int32,   RTAUDIO_SINT32 );
    sampleFormatMapper_s.add( SF_Float32, RTAUDIO_FLOAT32 );
    sampleFormatMapper_s.add( SF_Float64, RTAUDIO_FLOAT64 );
}



RtAudioDevice::RtAudioDevice(const AudioDeviceInfo& deviceInfo) : 
    AudioDevice(deviceInfo),
    rtAudio_(NULL)
{
    RtAudio::Api rtType = RtAudioBridge::portIdMapper_s.getRight(portId_);

    rtAudio_ = new RtAudio(rtType);
    rtAudio_->showWarnings(false);
}



RtAudioDevice::~RtAudioDevice()
{
    if(rtAudio_ != NULL)
        delete rtAudio_;
}



void RtAudioDevice::openCallbackStream(StreamCallback* callback, void* userData)
{
#ifdef _DEBUG
    rtAudio_->showWarnings( true );
#endif

	RtAudio::StreamParameters params;
    memset(&params, 0, sizeof(params)); 

	params.deviceId     = privateId_;
	params.nChannels    = numChannels_;
    params.firstChannel = 0;

	RtAudio::StreamOptions options;
	options.flags |= RTAUDIO_SCHEDULE_REALTIME;
	options.flags |= RTAUDIO_MINIMIZE_LATENCY;

    if(interleaved_ == false)
	    options.flags |= RTAUDIO_NONINTERLEAVED;

	try {
		rtAudio_->openStream( 
            &params, 
            NULL, 
            sampleFormatMapper_s.getRight(sampleFormat_), 
            sampleRate_, 
            &bufferSize_, 
            &rtAudioCallback, 
            this, 
            &options
        );
	}
	catch(const RtError& e) {
        EXCEPTION(std::exception, e.what());
	}

    streamCallback_ = callback;
    callbackData_   = userData;
}



int RtAudioDevice::rtAudioCallback(
    void* output, 
    void* input, 
    unsigned int numFrames, 
    double streamTime, 
    RtAudioStreamStatus status, 
    void* userData)
{
    RtAudioDevice* device = (RtAudioDevice*)userData;
    device->profiler_.begin();

    int result = device->streamCallback_(input, output, numFrames, 0, status, device->callbackData_);

    device->profiler_.end(numFrames);
    return result;
}



void RtAudioDevice::startCallbackStream()
{
    profiler_.reset();
    rtAudio_->startStream();
}



void RtAudioDevice::closeCallbackStream()
{
    profiler_.reset();
    rtAudio_->closeStream();
}



bool RtAudioDevice::isStreamActive() const
{
    return rtAudio_->isStreamRunning();
}



double RtAudioDevice::getStreamLatency() const
{
    if(isStreamActive() == false)
        return 0;

    long latency   = rtAudio_->getStreamLatency();
    int sampleRate = getStreamSampleRate();

    if(sampleRate == 0)
        return 0;

    return latency / (double)sampleRate;
}



int RtAudioDevice::getStreamSampleRate() const 
{
    if(isStreamActive() == false)
        return 0;

    return rtAudio_->getStreamSampleRate();
}



void RtAudioDevice::setSampleRate(int sampleRate)
{
    AudioDevice::setSampleRate(sampleRate);
    profiler_.setSampleRate(sampleRate);
}






