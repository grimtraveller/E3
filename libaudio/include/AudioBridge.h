
//----------------------------------------------------------------
// A bridge for audio libraries
// Provides information about the installed audio api's and
// available audio devices
//----------------------------------------------------------------

#pragma once

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include <portaudio.h>
#include <EnumHelper.h>


namespace e3 {

    class AudioDevice;
    class AudioDeviceInfo;

    typedef boost::shared_ptr<AudioDevice> AudioDevicePtr;
    typedef std::vector<AudioDeviceInfo> AudioDeviceInfoVector;


    enum AudioPortId
    {
        AP_UNSPECIFIED = paInDevelopment,      // Search for a working compiled API
        AP_ASIO = paASIO,               // Steinberg Audio Stream I/O
        AP_DS = paDirectSound,        // Microsoft Direct Sound
        AP_MME = paMME,                // Microsoft Multimedia Extensions
        AP_WASAPI = paWASAPI,             // Windows Audio Session
        AP_WDMKS = paWDMKS,              // Windows Driver Model Kernel Streaming
        AP_ALSA = paALSA,               // Advanced Linux Sound Architecture
        AP_OSS = paOSS,                // Linux Open Sound System
        AP_JACK = paJACK,               // Jack Low-Latency Audio Server
        AP_COREAUDIO = paCoreAudio,          // Macintosh OS-X Core Audio
        AP_SOUNDMANAGER = paSoundManager,       // SoundManager
        AP_BEOS = paBeOS,               // BeOS 
        AP_HPI = paAudioScienceHPI,    // AudioScience Hardware Programming Interface
    };


    class AudioBridge
    {
    public:
        enum {
            NoDevice = -1,
            DefaultDevice = -2
        };

    public:
        AudioBridge();
        virtual ~AudioBridge();

        typedef EnumNames<AudioPortId> PortNames;
        static const PortNames& getPortNames()    { return portNames_s; }

        const AudioDeviceInfoVector& getDevices() const { return devices_; }
        const AudioDeviceInfo& getDeviceInfo(int deviceId) const;

        AudioDevicePtr getDevice(int deviceId) const;
        AudioDevicePtr getDevice(const std::string& portName, const std::string& deviceName) const;
        AudioDevicePtr getInputDevice(int deviceId = DefaultDevice) const;
        AudioDevicePtr getOutputDevice(int deviceId = DefaultDevice) const;

    private:
        AudioDeviceInfoVector devices_;
        static PortNames portNames_s;

        struct Initializer { Initializer(); };
        static Initializer initializer_s;

        void findSupportedSampleRates(AudioDeviceInfo& deviceInfo) const;
    };

    typedef boost::shared_ptr<AudioBridge> AudioBridgePtr;


} // namespace e3


