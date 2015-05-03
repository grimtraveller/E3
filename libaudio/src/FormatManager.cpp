
//--------------------------------------------------------
// FormatManager.cpp
//--------------------------------------------------------

#include <algorithm>

#include <e3_CommonMacros.h>
#include <e3_Exception.h>

#include <FormatManager.h>
#include <MultiFormatAudioFile.h>
#include <MpegFile.h>


namespace e3 {

    FormatInfoVector FormatManager::formatInfos_;
    CodecInfoVector FormatManager::codecInfos_;

    FormatManager::Initializer FormatManager::initializer_s;

    FormatManager::Initializer::Initializer()
    {
        formatInfos_.reserve(FORMAT_COUNT);
        codecInfos_.reserve(CODEC_COUNT);

        MultiFormatAudioFile::initFormatInfos(formatInfos_);
        MultiFormatAudioFile::initCodecInfos(codecInfos_);

        MpegFile::initFormatInfos(formatInfos_);
        MpegFile::initCodecInfos(codecInfos_);
    }



    AudioFilePtr FormatManager::createFile(const Path& filename)
    {
        if (filename.extension().string() == ".mp3") {                          // TODO: put this information to FormatInfo
            return MpegFilePtr(new MpegFile());
        }
        else {
            return MultiFormatAudioFilePtr(new MultiFormatAudioFile());
        }
    }




    const FormatInfo& FormatManager::getFormat(FormatId id)
    {
        FormatInfoVector::iterator it =
            std::find_if(formatInfos_.begin(), formatInfos_.end(), boost::bind(&FormatInfo::id_, _1) == id);

        if (it == formatInfos_.end())
            THROW(std::exception, "Unknown format: %d", id);

        return *it;
    }


    const FormatInfo& FormatManager::getFormat(int idPrivate)
    {
        FormatInfoVector::iterator it =
            std::find_if(formatInfos_.begin(), formatInfos_.end(), boost::bind(&FormatInfo::idPrivate_, _1) == idPrivate);

        if (it == formatInfos_.end())
            THROW(std::exception, "Unknown format: %d", idPrivate);

        return *it;
    }



    const FormatInfo& FormatManager::getFormat(const std::string& name)
    {
        FormatInfoVector::iterator it =
            std::find_if(formatInfos_.begin(), formatInfos_.end(), boost::bind(&FormatInfo::name_, _1) == name);

        if (it == formatInfos_.end())
            THROW(std::exception, "Unknown format: %s", name.c_str());

        return *it;
    }

    const CodecInfo& FormatManager::getCodec(CodecId id)
    {
        CodecInfoVector::iterator it =
            std::find_if(codecInfos_.begin(), codecInfos_.end(), boost::bind(&CodecInfo::id_, _1) == id);

        if (it == codecInfos_.end())
            THROW(std::exception, "Unknown codec: %d", id);

        return *it;
    }


    const CodecInfo& FormatManager::getCodec(int idPrivate)
    {
        CodecInfoVector::iterator it =
            std::find_if(codecInfos_.begin(), codecInfos_.end(), boost::bind(&CodecInfo::idPrivate_, _1) == idPrivate);

        if (it == codecInfos_.end())
            THROW(std::exception, "Unknown codec: %d", idPrivate);

        return *it;
    }



    const CodecInfo& FormatManager::getCodec(const std::string& name)
    {
        CodecInfoVector::iterator it =
            std::find_if(codecInfos_.begin(), codecInfos_.end(), boost::bind(&CodecInfo::name_, _1) == name);

        if (it == codecInfos_.end())
            THROW(std::exception, "Unknown codec: %s", name.c_str());

        return *it;
    }



    bool FormatManager::isSupported(const FormatInfo& format, const CodecInfo& codec, int sampleRate, int numChannels)
    {
        bool result = false;
        result |= MultiFormatAudioFile::isFormatSupported(format, codec, sampleRate, numChannels);
        result |= MpegFile::isFormatSupported(format, codec, sampleRate, numChannels);

        return result;
    }

} // namespace e3