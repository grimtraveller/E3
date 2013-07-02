
//---------------------------------------------------
// Wrapper for linsndfile
//
// http://mega-nerd.com/libsndfile/api.html
//---------------------------------------------------

#pragma once

#include <stdio.h>

#include <vector>
#include <boost/unordered_map.hpp>
#include <boost/filesystem.hpp>
#include <boost/smart_ptr.hpp>

#include <sndfile.h>  // libsndfile

#include <EnumHelper.h>
#include <IntegerTypes.h>

#include "AudioFile.h"
#include "AudioFormat.h"


class AudioBuffer;
class InstrumentChunk;

//--------------------------------------------
// class MultiFormatAudioFile
//--------------------------------------------

class MultiFormatAudioFile : public AudioFile
{
public:
    MultiFormatAudioFile();
    ~MultiFormatAudioFile();

    void open(const Path& filename, FileOpenMode mode);
    void load(AudioBuffer* buffer);
    void store(const AudioBuffer* buffer);
    void close();
	int64 seek(int64 frame);
    
    int64 readShort(short* buffer, int64 num)       { return sf_read_short(handle_, buffer, num); }
    int64 readInt(int* buffer, int64 num)           { return sf_read_int(handle_, buffer, num); }
    int64 readFloat(float* buffer, int64 num)       { return sf_read_float(handle_, buffer, num); }
    int64 readDouble(double* buffer, int64 num)     { return sf_read_double(handle_, buffer, num); }

    int64 writeShort(short* buffer, int64 num)      { return sf_write_short(handle_, buffer, num); }
    int64 writeInt(int* buffer, int64 num)          { return sf_write_int(handle_, buffer, num); }
    int64 writeFloat(float* buffer, int64 num)      { return sf_write_float(handle_, buffer, num); }
    int64 writeDouble(double* buffer, int64 num)    { return sf_write_double(handle_, buffer, num); }

    SNDFILE* getHandle() const                      { return handle_; }
    bool isOpened() const                           { return handle_ != NULL; }
    int getNumSections() const                      { return numSections_; }

    static std::string getVersionString();
    static bool isFormatSupported(const FormatInfo& format, const CodecInfo& codec, int sampleRate=0, int numChannels=1);

protected:
    int makeSfFormat() const      { return format_.idPrivate_ & SF_FORMAT_TYPEMASK | codec_.idPrivate_; }
    void loadInstrumentChunk();
    void storeInstrumentChunk();

	int	numSections_;
    SNDFILE* handle_;

    friend class FormatManager;
    static void initFormatInfos(FormatInfoVector& infos);
    static void initCodecInfos(CodecInfoVector& infos);
};

typedef boost::shared_ptr<MultiFormatAudioFile> MultiFormatAudioFilePtr;



