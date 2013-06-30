
//--------------------------------------------------------
// MpegFile.h
//--------------------------------------------------------

#pragma once

#include <boost/smart_ptr.hpp>
#include <mad.h>

#include <Buffer.h>
#include <AudioFile.h>

class AudioBuffer;


class MpegFile : public AudioFile
{
friend class FormatManager;
public:
    MpegFile();
    ~MpegFile();

    void open(const Path& filename, FileOpenMode mode);
    void load(AudioBuffer* buffer);
    void store(const AudioBuffer* buffer)               {}
    void close();
    int64 seek(int64 frame)                             { return 0; }

    bool isOpened() const                               { return handle_ != NULL; }
    bool isSeekable() const;
    
    std::string getVersionString() const                { return mad_version; }
    static bool isFormatSupported(const FormatInfo& format, const CodecInfo& codec, int sampleRate=0, int numChannels=1);

protected:
    FILE* handle_;
    friend class FormatManager;
    static void initFormatInfos(FormatInfoVector& infos);
    static void initCodecInfos(CodecInfoVector& infos);

    int64 getDuration();
    void fileRead();
    size_t mpegRead(size_t len, AudioBuffer* buffer);
    bool consumeId3Tag();

    typedef Buffer<unsigned char> MpegBuffer;
    MpegBuffer mpegBuffer_;
    int currentSample_;
    size_t signalLength_;

    struct mad_stream madStream_;
    struct mad_frame  madFrame_;
    struct mad_synth  madSynth_;
    mad_timer_t       madTimer_;

    static unsigned long xingFrames(struct mad_bitptr ptr, unsigned bitlen);
    static void madTimerMultiply(mad_timer_t* t, double d);
    static int getId3TagSize(const unsigned char* data, size_t length);
};

typedef boost::shared_ptr<MpegFile> MpegFilePtr;
