
//--------------------------------------------------------
// MpegFile.h
//--------------------------------------------------------

#pragma once

#include <stdio.h>

#include <boost/smart_ptr.hpp>
#include <mad.h>

#include <IntegerTypes.h>
#include <AudioFile.h>

class AudioBuffer;
class MadDecoder;


class MpegFile : public AudioFile
{
friend class FormatManager;
public:
    MpegFile();
    ~MpegFile();

    void open(const Path& filename, FileOpenMode mode);
    void load(AudioBuffer* buffer);
    void store(const AudioBuffer* buffer)               { EXCEPTION(std::exception, "Storing not implemented for MPEG"); }
    void close();
    bool isOpened() const                               { return handle_ != NULL; }
    
    std::string getVersionString() const                { return mad_version; }
    static bool isFormatSupported(const FormatInfo& format, const CodecInfo& codec, int sampleRate=0, int numChannels=1);

protected:
    FILE* handle_;
    MadDecoder* decoder_;
    friend class FormatManager;
    static void initFormatInfos(FormatInfoVector& infos);
    static void initCodecInfos(CodecInfoVector& infos);
};

typedef boost::shared_ptr<MpegFile> MpegFilePtr;
