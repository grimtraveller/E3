
#include <boost/foreach.hpp>

#include <LibCommon.h>

#include <AudioBridge.h>
#include <AudioDevice.h>



AudioBridge::PortNames AudioBridge::portNames_s;
AudioBridge::Initializer AudioBridge::initializer_s;


AudioBridge::Initializer::Initializer()
{
    portNames_s.add(AP_UNSPECIFIED,  "UNSPECIFIED",  "Search for a working compiled API");
    portNames_s.add(AP_ASIO,         "ASIO",         "Steinberg Audio Stream I/O");
    portNames_s.add(AP_DS,           "DS",           "Microsoft Direct Sound");
    portNames_s.add(AP_MME,          "MME",          "Microsoft Multimedia Extensions");
    portNames_s.add(AP_WASAPI,       "WASAPI",       "Windows Audio Session");
    portNames_s.add(AP_WDMKS,        "WDMKS",        "Windows Driver Model Kernel Streaming");
    portNames_s.add(AP_ALSA,         "ALSA",         "Advanced Linux Sound Architecture");
    portNames_s.add(AP_OSS,          "OSS",          "Linux Open Sound System");
    portNames_s.add(AP_JACK,         "JACK",         "Jack Low-Latency Audio Server");
    portNames_s.add(AP_COREAUDIO,    "COREAUDIO",    "Macintosh OS-X Core Audio");
    portNames_s.add(AP_SOUNDMANAGER, "SOUNDMANAGER", "SoundManager");
    portNames_s.add(AP_BEOS,         "BEOS",         "BeOS"); 
    portNames_s.add(AP_HPI,          "HPI",          "AudioScience Hardware Programming Interface");
}



AudioBridge::AudioBridge()
{
    PaError error = Pa_Initialize();
	if(error != paNoError) 
		EXCEPTION(std::exception, Pa_GetErrorText(error));

    PaDeviceIndex paDefaultInDevice  = Pa_GetDefaultInputDevice(); 
    PaDeviceIndex paDefaultOutDevice = Pa_GetDefaultOutputDevice(); 

    for(int idxPort=0; idxPort<Pa_GetHostApiCount(); idxPort++)
    {
        const PaHostApiInfo* paInfo = Pa_GetHostApiInfo(idxPort);

        if(paInfo != NULL) 
        {
            AudioPortId portId = static_cast<AudioPortId>(paInfo->type);
            int paApiIndex     = Pa_HostApiTypeIdToHostApiIndex(paInfo->type);

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



AudioBridge::~AudioBridge()
{
    PaError error = Pa_Terminate();
	if(error != paNoError) 
        EXCEPTION(std::exception, Pa_GetErrorText(error));
}



const AudioDeviceInfo& AudioBridge::getDeviceInfo(int deviceId) const
{
	BOOST_FOREACH (const AudioDeviceInfo& info, devices_)   
    {
        if(info.getDeviceId() == deviceId || info.isDefault() && deviceId == DefaultDevice) {
            return info;
        }
    }
    EXCEPTION(std::exception, "device not found (device: %d)", deviceId);
}



AudioDevicePtr AudioBridge::getDevice(int deviceId) const
{
    const AudioDeviceInfo& deviceInfo = getDeviceInfo(deviceId);
    return AudioDevicePtr(new AudioDevice(deviceInfo));
}



AudioDevicePtr AudioBridge::getDevice(const std::string& portName, const std::string& deviceName) const
{
	BOOST_FOREACH (const AudioDeviceInfo& info, devices_)   
    {
        if(info.getPortName() == portName && info.getDeviceName() == deviceName)
        {
            return getDevice(info.getDeviceId());
        }
    }
    EXCEPTION(std::exception, "no such device: %s)", deviceName);
}



AudioDevicePtr AudioBridge::getInputDevice(int deviceId) const
{
	BOOST_FOREACH (const AudioDeviceInfo& info, devices_)   
    {
        if(info.isInput()) {
            if(info.getDeviceId() == deviceId || info.isDefault() && deviceId == DefaultDevice) {
                return getDevice(info.getDeviceId());
            }
        }
    }
    EXCEPTION(std::exception, "device not found (device: %d)", deviceId);
}



AudioDevicePtr AudioBridge::getOutputDevice(int deviceId) const
{
	BOOST_FOREACH (const AudioDeviceInfo& info, devices_)   
    {
        if(info.isOutput()) {
            if(info.getDeviceId() == deviceId || info.isDefault() && deviceId == DefaultDevice) {
                return getDevice(info.getDeviceId());
            }
        }
    }
    EXCEPTION(std::exception, "device not found (device: %d)", deviceId);
}



void AudioBridge::findSupportedSampleRates(AudioDeviceInfo& deviceInfo) const
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




