
//---------------------------------------------------
// Wrapper for linsndfile
//
// http://mega-nerd.com/libsndfile/api.html
//---------------------------------------------------

#pragma once

#include <sndfile.h>  // libsndfile

#include <vector>
#include <boost/unordered_map.hpp>
#include <boost/filesystem.hpp>
#include <boost/smart_ptr.hpp>

#include <EnumHelper.h>
#include <IntegerTypes.h>


class AudioBuffer;

//--------------------------------------------
// class AudioFile
//--------------------------------------------

class AudioFile
{
public:
    enum Format
    {
        FMT_UNKNOWN = -1,
	    FMT_WAV     = SF_FORMAT_WAV,      // Microsoft WAV format (little endian default).
	    FMT_AIFF    = SF_FORMAT_AIFF,     // Apple/SGI AIFF format (big endian). 
	    FMT_AU      = SF_FORMAT_AU,       // Sun/NeXT AU format (big endian). 
	    FMT_RAW     = SF_FORMAT_RAW,      // RAW PCM data. 
	    FMT_PAF     = SF_FORMAT_PAF,      // Ensoniq PARIS file format. 
	    FMT_SVX     = SF_FORMAT_SVX,      // Amiga IFF / SVX8 / SV16 format. 
	    FMT_NIST    = SF_FORMAT_NIST,     // Sphere NIST format. 
	    FMT_VOC     = SF_FORMAT_VOC,      // VOC files. 
	    FMT_IRCAM   = SF_FORMAT_IRCAM,    // Berkeley/IRCAM/CARL 
	    FMT_W64     = SF_FORMAT_W64,      // Sonic Foundry's 64 bit RIFF/WAV 
	    FMT_MAT4    = SF_FORMAT_MAT4,     // Matlab (tm) V4.2 / GNU Octave 2.0 
	    FMT_MAT5    = SF_FORMAT_MAT5,     // Matlab (tm) V5.0 / GNU Octave 2.1 
	    FMT_PVF     = SF_FORMAT_PVF,      // Portable Voice Format 
	    FMT_XI      = SF_FORMAT_XI,       // Fasttracker 2 Extended Instrument 
	    FMT_HTK     = SF_FORMAT_HTK,      // HMM Tool Kit format 
	    FMT_SDS     = SF_FORMAT_SDS,      // Midi Sample Dump Standard 
	    FMT_AVR     = SF_FORMAT_AVR,      // Audio Visual Research 
	    FMT_WAVEX   = SF_FORMAT_WAVEX,    // MS WAVE with WAVEFORMATEX 
	    FMT_SD2     = SF_FORMAT_SD2,      // Sound Designer 2 
	    FMT_FLAC    = SF_FORMAT_FLAC,     // FLAC lossless file format 
	    FMT_CAF     = SF_FORMAT_CAF,      // Core Audio File format 
	    FMT_WVE     = SF_FORMAT_WVE,      // Psion WVE format 
	    FMT_OGG     = SF_FORMAT_OGG,      // Xiph OGG container 
	    FMT_MPC2K   = SF_FORMAT_MPC2K,    // Akai MPC 2000 sampler 
	    FMT_RF64    = SF_FORMAT_RF64,     // RF64 WAV file 
    };

    enum Encoding
    {
	    ENC_UNKNOWN     = -1,
        ENC_PCM_S8      = SF_FORMAT_PCM_S8,      // Signed 8 bit data 
	    ENC_PCM_16      = SF_FORMAT_PCM_16,      // Signed 16 bit data
	    ENC_PCM_24      = SF_FORMAT_PCM_24,      // Signed 24 bit data
	    ENC_PCM_32      = SF_FORMAT_PCM_32,      // Signed 32 bit data

        ENC_PCM_U8      = SF_FORMAT_PCM_U8,      // Unsigned 8 bit data (WAV and RAW only) 

        ENC_FLOAT       = SF_FORMAT_FLOAT,       // 32 bit float data 
	    ENC_DOUBLE      = SF_FORMAT_DOUBLE,      // 64 bit float data 

        ENC_ULAW        = SF_FORMAT_ULAW,        // U-Law encoded. 
	    ENC_ALAW 	    = SF_FORMAT_ALAW,        // A-Law encoded. 
	    ENC_IMA_ADPCM   = SF_FORMAT_IMA_ADPCM,   // IMA ADPCM. 
	    ENC_MS_ADPCM    = SF_FORMAT_MS_ADPCM,    // Microsoft ADPCM

        ENC_GSM610      = SF_FORMAT_GSM610,      // GSM 6.10 encoding. 
	    ENC_VOX_ADPCM   = SF_FORMAT_VOX_ADPCM,   // OKI / Dialogix ADPCM 

        ENC_G721_32     = SF_FORMAT_G721_32,     // 32kbs G721 ADPCM encoding
	    ENC_G723_24     = SF_FORMAT_G723_24, 	 // 24kbs G723 ADPCM encoding
	    ENC_G723_40     = SF_FORMAT_G723_40, 	 // 40kbs G723 ADPCM encoding

        ENC_DWVW_12     = SF_FORMAT_DWVW_12,	 // 12 bit Delta Width Variable Word encoding
	    ENC_DWVW_16     = SF_FORMAT_DWVW_16, 	 // 16 bit Delta Width Variable Word encoding
	    ENC_DWVW_24     = SF_FORMAT_DWVW_24, 	 // 24 bit Delta Width Variable Word encoding
	    ENC_DWVW_N      = SF_FORMAT_DWVW_N, 	 // N bit Delta Width Variable Word encoding

        ENC_DPCM_8      = SF_FORMAT_DPCM_8, 	 // 8 bit differential PCM (XI only) 
	    ENC_DPCM_16     = SF_FORMAT_DPCM_16,     // 16 bit differential PCM (XI only) 

        ENC_VORBIS      = SF_FORMAT_VORBIS,      // Xiph Vorbis encoding. 
    };

    enum FileOpenMode 
    {
        OpenRead  = 0,
        OpenWrite = 1,
        OpenRdwr  = 2
    };

    class InstrumentChunk
    {
    friend class AudioFile;
    public:
        enum LoopMode 
        {
            LoopNone        = SF_LOOP_NONE,
            LoopForward     = SF_LOOP_FORWARD,
            LoopBackward    = SF_LOOP_BACKWARD,
            LoopAlternating = SF_LOOP_ALTERNATING
        };

        struct LoopData
		{
			LoopData();

            LoopMode mode_;
			uint32 start_;
			uint32 end_;
			uint32 numRepeats_;
		};
		typedef std::vector<LoopData> LoopVector;

        InstrumentChunk();
        
        void setHasData(bool hasData)           { hasData_ = hasData; }
        bool hasData() const                    { return hasData_; }

        void setGain(int gain)                  { hasData_ = true; gain_ = gain; }
        void setBaseNote(int baseNote)          { hasData_ = true; baseNote_ = baseNote; }
        void setKeyLow(int keyLow)              { hasData_ = true; keyLow_ = keyLow; }
        void setKeyHigh(int keyHigh)            { hasData_ = true; keyHigh_ = keyHigh; }
        void setVelocityLow(int velocityLow)    { hasData_ = true; velocityLow_ = velocityLow; }
        void setVelocityHigh(int velocityHigh)  { hasData_ = true; velocityHigh_ = velocityHigh; }
        void setDetune(int detune)              { hasData_ = true; detune_ = detune; }
        
        int getGain() const                     { return gain_; }
        int getBaseNote() const                 { return baseNote_; }
        int getKeyLow() const                   { return keyLow_; }
        int getKeyHigh() const                  { return keyHigh_; }
        int getVelocityLow() const              { return velocityLow_; }
        int getVelocityHigh() const             { return velocityHigh_; }
        int getDetune() const                   { return detune_; }

        void clearLoops()                       { loops_.clear(); }
        void addLoop(const LoopData& data)      { hasData_ = true; loops_.push_back(data); }
        int getNumLoops() const                 { return loops_.size(); }
        const LoopVector& getLoops() const      { return loops_; }

        typedef EnumNames<LoopMode> LoopModeInfo;
        static LoopModeInfo& getLoopModeInfo()  { return loopModeInfo_s; }

    protected:
        bool hasData_;
        int gain_;
        int baseNote_;
        int keyLow_;
        int keyHigh_;
        int velocityLow_;
        int velocityHigh_;
        int detune_;
		LoopVector loops_;

    protected:
        //! Maps the LoopMode constants to strings. 
        static LoopModeInfo loopModeInfo_s;
        struct Initializer {
            Initializer();
        };
        static Initializer initializer_s;
    };

typedef boost::filesystem::path Path;

public:
    AudioFile();
    ~AudioFile();

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

    bool isOpen() const                     { return handle_ != NULL; }
    bool hasError() const                   { return sf_error(handle_) != SF_ERR_NO_ERROR; }
    bool isReadable() const                 { return isOpen() && (fileOpenMode_ == OpenRead  || fileOpenMode_ == OpenRdwr); }
    bool isWriteable() const                { return isOpen() && (fileOpenMode_ == OpenWrite || fileOpenMode_ == OpenRdwr); }
    bool checkFormat() const;

    std::string getErrorString()            { return sf_strerror(handle_); }
    std::string getVersionString() const;
    
    const Path& getFilename() const         { return filename_; }
    void setFilename(const Path& filename)  { filename_ = filename; }  

    int getSampleRate() const               { return sampleRate_; }
    int getNumChannels() const              { return numChannels_; }
    int getNumSections() const              { return numSections_; }
    Format getFormat() const                { return format_; }
    Encoding getEncoding() const            { return encoding_; }
    int64 getNumFrames() const              { return numFrames_; }

    void setSampleRate(int sampleRate)      { sampleRate_ = sampleRate; }
    void setNumChannels(int numChannels)    { numChannels_ = numChannels; }
    void setFormat(Format format)           { format_ = format; }
    void setEncoding(Encoding encoding)     { encoding_ = encoding; }

    float getDurationSec() const			{ return (float)numFrames_ / (float)sampleRate_; }
    float getDurationMs() const				{ return (float)numFrames_ / (float)sampleRate_ / 1000; }

	virtual InstrumentChunk& getInstrumentChunk()   { return instrumentChunk_; }

    typedef EnumNames<Format> FormatNames;
    static const FormatNames& getFormatNames()      { return formatNames_s; }

    typedef EnumNames<Encoding> EncodingNames;
    static const EncodingNames& getEncodingNames()  { return encodingNames_s; }

protected:
    int packFormat() const      { return format_ | encoding_; }
    void loadInstrumentChunk();
    void storeInstrumentChunk();

    int sampleRate_;
    int numChannels_;
	int	numSections_;
    int64 numFrames_;

    Path filename_;
    FileOpenMode fileOpenMode_;
    SNDFILE* handle_;

    Format format_;
    Encoding encoding_;
	bool seekable_;
	InstrumentChunk instrumentChunk_;

    static FormatNames formatNames_s;
    static EncodingNames encodingNames_s;
    
    struct Initializer {
        Initializer();
    };
    static Initializer initializer_s;
};

typedef boost::shared_ptr<AudioFile> AudioFilePtr;



