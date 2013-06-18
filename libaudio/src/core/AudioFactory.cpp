

#include <boost/algorithm/string.hpp>

#include <Macros.h>

#include <wrapper/PortAudioBridge.h>
#include <wrapper/RtAudioBridge.h>

#include <core/AudioFactory.h>


//AudioFile*     AudioFactory::createAudioFile()        { return new AudioFile(); }
//AudioFilePtr   AudioFactory::createAudioFilePtr()     { return AudioFilePtr(new AudioFile()); }

    
AudioFactory::AudioBridgeNames AudioFactory::audioBridgeNames_s;
AudioFactory::Initializer AudioFactory::initializer_s;


AudioFactory::Initializer::Initializer()
{
    audioBridgeNames_s.add(PortAudio, "PORTAUDIO");
    audioBridgeNames_s.add(RtAudio, "RTAUDIO");
}



AudioBridge* AudioFactory::createAudioBridge(AudioBridgeLib libId)      
{ 
    switch(libId) 
    {
    case PortAudio: return new PortAudioBridge();
    case RtAudio:   return new RtAudioBridge();
    default:        break;
    }
    EXCEPTION(std::exception, "invalid libId");
}


AudioBridgePtr AudioFactory::createAudioBridgePtr(AudioBridgeLib libId)   
{ 
    AudioBridge* audioBridge = createAudioBridge(libId);
    return AudioBridgePtr(audioBridge); 
}


AudioBridgePtr AudioFactory::createAudioBridgePtr(const std::string& libName)
{
    std::string uppercaseName = libName;
    boost::algorithm::to_upper(uppercaseName);

    AudioBridgeLib lib = uppercaseName.empty() ?        // select library
        PortAudio :                                     // default
        audioBridgeNames_s.getValue(uppercaseName);     // explicit

    return createAudioBridgePtr(lib);
}

