
//---------------------------------------------------------------------------
// Declarations for factory functions to create objects for audio resources.
//
// The declarations are indepent from the implementation, which may reside in
// arbitrary libraries or bridges to libraries.
//----------------------------------------------------------------------------

#pragma once

#include <string>
#include <boost/smart_ptr.hpp>
#include <EnumHelper.h>


class AudioFile;
class AudioBridge;

typedef boost::shared_ptr<AudioFile> AudioFilePtr;
typedef boost::shared_ptr<AudioBridge> AudioBridgePtr;


class AudioFactory 
{
public:
    enum AudioBridgeLib {
        PortAudio = 1,
        RtAudio   = 2
    };

    //static AudioFile*     createAudioFile();
    //static AudioFilePtr   createAudioFilePtr();
    static AudioBridge*   createAudioBridge(AudioBridgeLib libId=PortAudio);
    static AudioBridgePtr createAudioBridgePtr(AudioBridgeLib libId=PortAudio);
    static AudioBridgePtr createAudioBridgePtr(const std::string& libName);

    typedef EnumNames<AudioBridgeLib> AudioBridgeNames;
    static const AudioBridgeNames& getAudioBridgeNames()  { return audioBridgeNames_s; }

protected:
    static AudioBridgeNames audioBridgeNames_s;

    struct Initializer {
        Initializer();
    };
    static Initializer initializer_s;
};

