
#include <boost/foreach.hpp>

#include <Macros.h>

#include <core/AudioBridge.h>
#include <core/AudioDevice.h>


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
    portNames_s.add(AP_PULSE,        "PULSE",        "Linux PulseAudio");
    portNames_s.add(AP_OSS,          "OSS",          "Linux Open Sound System");
    portNames_s.add(AP_JACK,         "JACK",         "Jack Low-Latency Audio Server");
    portNames_s.add(AP_COREAUDIO,    "COREAUDIO",    "Macintosh OS-X Core Audio");
    portNames_s.add(AP_SOUNDMANAGER, "SOUNDMANAGER", "SoundManager");
    portNames_s.add(AP_BEOS,         "BEOS",         "BeOS"); 
    portNames_s.add(AP_HPI,          "HPI",          "AudioScience Hardware Programming Interface");
}



const AudioDeviceInfo& AudioBridge::getDeviceInfo(int deviceId) const
{
	BOOST_FOREACH (const AudioDeviceInfo& info, devices_)   
    {
        if(info.getDeviceId() == deviceId || info.isDefault() && deviceId == DefaultDevice) {
            return info;
        }
    }
    EXCEPTION(std::exception, "device not found (device: %d)\n", deviceId);
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
    EXCEPTION(std::exception, "no such device: %s)\n", deviceName);
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
    EXCEPTION(std::exception, "device not found (device: %d)\n", deviceId);
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
    EXCEPTION(std::exception, "device not found (device: %d)\n", deviceId);
}



//const AudioDeviceInfo& AudioBridge::getDeviceInfo(AudioPortId portId, int deviceId) const
//{
//    if(portId == AP_UNSPECIFIED) {
//		portId = getDefaultPort();
//	}
//	if(deviceId == DefaultDevice) {
//		deviceId = getDefaultOutputDevice();
//	}
//
//	for(AudioDeviceInfoVector::const_iterator it=devices_.begin(); it!=devices_.end(); ++it)
//    {
//        const AudioDeviceInfo& info = *it;
//        if(info.getPortId() == portId && info.getDeviceId() == deviceId)
//        {
//            return info;
//        }
//    }
//    EXCEPTION(std::exception, "device not found (port: %d, device: %d)\n", portId, deviceId);
//}


/*
AudioDevicePtr AudioBridge::getInputDevice(AudioPortId portId, int deviceId) const
{
    if(portId == AP_UNSPECIFIED && deviceId == DefaultDevice) 
    {
        int defaultInputIdx = getDefaultInputDevice();
        const AudioDeviceInfo& info = devices_.at(defaultInputIdx);

        return getDevice(info.getPortId(), info.getDeviceId());
    }
    else return getDevice(portId, deviceId);
}



AudioDevicePtr AudioBridge::getOutputDevice(AudioPortId portId, int deviceId) const
{
    if(portId == AP_UNSPECIFIED && deviceId == DefaultDevice) 
    {
        int defaultOutputIdx = getDefaultOutputDevice();
        const AudioDeviceInfo& info = devices_.at(defaultOutputIdx);

        return getDevice(info.getPortId(), info.getDeviceId());
    }
    else return getDevice(portId, deviceId);
}
*/



