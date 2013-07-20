
//-----------------------------------------------
// AudioFile.h
//-----------------------------------------------

#pragma once

#include <boost/filesystem.hpp>
typedef boost::filesystem::path Path;

#include <boost/smart_ptr.hpp>

#include <CommonMacros.h>
#include <IntegerTypes.h>
#include <AudioFormat.h>

class AudioBuffer;
class InstrumentChunk;



class AudioFile
{
public:
    enum FileOpenMode 
    {
        OpenRead  = 0,
        OpenWrite = 1,
        OpenRdwr  = 2
    };

public:
    AudioFile();
    virtual ~AudioFile();

    virtual void open(const Path& filename, FileOpenMode mode);
    virtual void load(AudioBuffer* buffer)                     = 0;
    virtual void store(const AudioBuffer* buffer)              = 0;
    virtual void close()                                       = 0;
    virtual int64 seek(int64 frame)                            { return 0; }

    virtual void setFormat(const FormatInfo& format)    { format_ = format; }
    virtual void setCodec(const CodecInfo& codec)       { codec_  = codec; }
    virtual const FormatInfo& getFormat() const         { return format_; }
    virtual const CodecInfo& getCodec() const           { return codec_; }

    const Path& getFilename() const                     { return filename_; }
    void setFilename(const Path& filename)              { filename_ = filename; }  

    virtual int getSampleRate() const                   { return sampleRate_; }
    virtual int getNumChannels() const                  { return numChannels_; }
    virtual void setSampleRate(int sampleRate)          { sampleRate_ = sampleRate; }
    virtual void setNumChannels(int numChannels)        { numChannels_ = numChannels; }

    virtual int64 getNumFrames() const                  { return numFrames_; }
    virtual float getDurationSec() const    		    { return (float)numFrames_ / (float)sampleRate_; }

    virtual bool isOpened() const                       { return false; }
    virtual bool isReadable() const                     { return isOpened() && (fileOpenMode_ == OpenRead  || fileOpenMode_ == OpenRdwr); }
    virtual bool isWriteable() const                    { return isOpened() && (fileOpenMode_ == OpenWrite || fileOpenMode_ == OpenRdwr); }

    virtual std::string getVersionString() const        { return ""; }

	virtual InstrumentChunk* getInstrumentChunk()       { return instrumentChunk_; }

protected:
    FormatInfo format_;
    CodecInfo codec_;
    int sampleRate_;
    int numChannels_;
    int64 numFrames_;
    Path filename_;
    FileOpenMode fileOpenMode_;

    InstrumentChunk* instrumentChunk_;
};

typedef boost::shared_ptr<AudioFile> AudioFilePtr;