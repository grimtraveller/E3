
//---------------------------------------------------
// Bridge libsndfile/AudioFile
//---------------------------------------------------
// http://mega-nerd.com/libsndfile/api.html
//---------------------------------------------------

#pragma once

#include <sndfile.h>  // libsndfile

#include <core/AudioFile.h>


class LibSndFile : public AudioFile
{
public:
    LibSndFile();

    void open(const Path& filename, FileOpenMode mode);
	void close();
	int64 seek(int64 frame);
    bool checkFormat() const;

    int64 readShort(short* buffer, int64 num)       { return sf_read_short(handle_, buffer, num); }
    int64 readInt(int* buffer, int64 num)           { return sf_read_int(handle_, buffer, num); }
    int64 readFloat(float* buffer, int64 num)       { return sf_read_float(handle_, buffer, num); }
    int64 readDouble(double* buffer, int64 num)     { return sf_read_double(handle_, buffer, num); }

    int64 writeShort(short* buffer, int64 num)      { return sf_write_short(handle_, buffer, num); }
    int64 writeInt(int* buffer, int64 num)          { return sf_write_int(handle_, buffer, num); }
    int64 writeFloat(float* buffer, int64 num)      { return sf_write_float(handle_, buffer, num); }
    int64 writeDouble(double* buffer, int64 num)    { return sf_write_double(handle_, buffer, num); }

    bool isOpen() const                             { return handle_ != NULL; }
    bool hasError() const                           { return sf_error(handle_) != SF_ERR_NO_ERROR; }
    std::string getErrorString() const              { return sf_strerror(handle_); }
    std::string getVersionString() const;

    void loadInstrumentChunk();
    void storeInstrumentChunk();

protected:
    SNDFILE* handle_;

    int makeSFFormat() const  { return format_ | encoding_; }

    /*! Helper class to perform static intialization */
    class Initializer {
    public:
        Initializer();
    };
    static Initializer initializer_s;

    typedef EnumMapper<Format, int> FormatMapper;
    static FormatMapper formatMapper_s;

    typedef EnumMapper<Encoding, int> EncodingMapper;
    static EncodingMapper encodingMapper_s;
    
    typedef EnumMapper<InstrumentChunk::LoopMode, int> LoopModeMapper;
    static LoopModeMapper loopModeMapper_s;
};
