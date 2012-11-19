
//----------------------------------------------------------------
// A bridge for audio libraries
// Provides information about the installed audio api's and
// available audio devices
//----------------------------------------------------------------

#pragma once

#include <string>
#include <vector>
#include <boost/smart_ptr.hpp>

#include <EnumHelper.h>


class AudioDevice;  
class AudioDeviceInfo;

typedef boost::shared_ptr<AudioDevice> AudioDevicePtr;
typedef std::vector<AudioDeviceInfo> AudioDeviceInfoVector;


enum AudioPortId
{
    AP_UNSPECIFIED   =  0,     // Search for a working compiled API
    AP_ASIO          =  1,     // Steinberg Audio Stream I/O
    AP_DS            =  2,     // Microsoft Direct Sound
    AP_MME           =  3,     // Microsoft Multimedia Extensions
    AP_WASAPI        =  4,     // Windows Audio Session
    AP_WDMKS         =  5,     // Windows Driver Model Kernel Streaming
    AP_ALSA          =  6,     // Advanced Linux Sound Architecture
    AP_PULSE         =  7,     // Linux PulseAudio
    AP_OSS           =  8,     // Linux Open Sound System
    AP_JACK          =  9,     // Jack Low-Latency Audio Server
    AP_COREAUDIO     = 10,     // Macintosh OS-X Core Audio
    AP_SOUNDMANAGER  = 11,     // SoundManager
    AP_BEOS          = 12,     // BeOS 
    AP_HPI           = 13      // AudioScience Hardware Programming Interface
};


class AudioBridge
{
public:
	enum {
		NoDevice      = -1,
		DefaultDevice = -2
	};

public:
    AudioBridge() {}
    virtual ~AudioBridge() {}

    typedef EnumNames<AudioPortId> PortNames;
    static const PortNames& getPortNames()    { return portNames_s; }

    virtual const AudioDeviceInfoVector& getDevices() const { return devices_; }
    virtual const AudioDeviceInfo& getDeviceInfo(int deviceId) const;

    virtual AudioDevicePtr getDevice(int deviceId) const = 0;
    virtual AudioDevicePtr getDevice(const std::string& portName, const std::string& deviceName) const;
    virtual AudioDevicePtr getInputDevice(int deviceId=DefaultDevice) const;
    virtual AudioDevicePtr getOutputDevice(int deviceId=DefaultDevice) const;

protected:
    AudioDeviceInfoVector devices_;
    static PortNames portNames_s;

    struct Initializer { Initializer(); };
    static Initializer initializer_s;
};






