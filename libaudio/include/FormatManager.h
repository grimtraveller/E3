
//---------------------------------------------------
// FormatManager.h
//---------------------------------------------------

#pragma once

#include <string>
#include <vector>

#include <AudioFormat.h>
#include <AudioFile.h>


namespace e3 {

    class FormatManager
    {
    public:
        static AudioFilePtr createFile(const Path& filename);

        static const FormatInfoVector& getFormatInfos() { return formatInfos_; }
        static const CodecInfoVector& getCodecInfos()   { return codecInfos_; }

        static const FormatInfo& getFormat(FormatId id);
        static const FormatInfo& getFormat(int idPrivate);
        static const FormatInfo& getFormat(const std::string& name);

        static const CodecInfo& getCodec(CodecId id);
        static const CodecInfo& getCodec(int idPrivate);
        static const CodecInfo& getCodec(const std::string& name);

        static bool isSupported(const FormatInfo& format, const CodecInfo& codec, int sampleRate = 0, int numChannels = 1);

    protected:
        static FormatInfoVector formatInfos_;
        static CodecInfoVector codecInfos_;

        struct Initializer {
            Initializer();
        };
        static Initializer initializer_s;
    };

} // namespace e3