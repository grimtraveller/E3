
#pragma once

#include <vector>
#include <boost/unordered_map.hpp>
#include <boost/filesystem.hpp>

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
        FMT_UNKNOWN  = -1,
		FMT_WAV		 = 1,			// Microsoft WAV format (little endian default). 
		FMT_AIFF	 = 2,			// Apple/SGI AIFF format (big endian). 
        FMT_AIFC     = 3,
		FMT_AU		 = 4,			// Sun/NeXT AU format (big endian). 
		FMT_RAW		 = 5,			// RAW PCM data. 
		FMT_PAF		 = 6,			// Ensoniq PARIS file format. 
		FMT_SVX		 = 7,			// Amiga IFF / SVX8 / SV16 format. 
		FMT_NIST	 = 8,			// Sphere NIST format. 
		FMT_VOC		 = 9,			// VOC files. 
		FMT_IRCAM	 = 10,			// Berkeley/IRCAM/CARL 
		FMT_W64		 = 11,			// Sonic Foundry's 64 bit RIFF/WAV 
		FMT_MAT4	 = 12,			// Matlab (tm) V4.2 / GNU Octave 2.0 
		FMT_MAT5	 = 13,			// Matlab (tm) V5.0 / GNU Octave 2.1 
		FMT_PVF		 = 14,			// Portable Voice Format 
		FMT_XI		 = 15,			// Fasttracker 2 Extended Instrument 
		FMT_HTK		 = 16,			// HMM Tool Kit format 
		FMT_SDS		 = 17,			// Midi Sample Dump Standard 
		FMT_AVR		 = 18,			// Audio Visual Research 
		FMT_WAVEX	 = 19,			// MS WAVE with WAVEFORMATEX 
		FMT_SD2		 = 20,			// Sound Designer 2 
		FMT_FLAC	 = 21,			// FLAC lossless file format 
		FMT_CAF		 = 22,			// Core Audio File format 
		FMT_WVE		 = 23,			// Psion WVE format 
		FMT_OGG		 = 24,			// Xiph OGG container 
		FMT_MPC2K	 = 25,			// Akai MPC 2000 sampler 
		FMT_RF64	 = 26,			// RF64 WAV file 
	};

    enum Encoding 
    {
        ENC_UNKNOWN       = -1,
        ENC_PCM_S8		  = 1,			// Signed 8 bit data 
		ENC_PCM_16		  = 2,			// Signed 16 bit data 
		ENC_PCM_24		  = 3,			// Signed 24 bit data 
		ENC_PCM_32		  = 4,			// Signed 32 bit data 

        ENC_PCM_U8		  = 5,			// Unsigned 8 bit data (WAV and RAW only) 

        ENC_FLOAT		  = 6,			// 32 bit float data 
		ENC_DOUBLE		  = 7,			// 64 bit float data 

        ENC_ULAW		  = 8,			// U-Law encoded. 
		ENC_ALAW		  = 9,			// A-Law encoded. 
		ENC_IMA_ADPCM	  = 10,			// IMA ADPCM. 
		ENC_MS_ADPCM	  = 11,			// Microsoft ADPCM. 

        ENC_GSM610		  = 12,			// GSM 6.10 encoding. 
		ENC_VOX_ADPCM	  = 13,		    // OKI / Dialogix ADPCM 

        ENC_G721_32		  = 14,			// 32kbs G721 ADPCM encoding. 
		ENC_G723_24		  = 15,			// 24kbs G723 ADPCM encoding. 
		ENC_G723_40		  = 16,			// 40kbs G723 ADPCM encoding. 

        ENC_DWVW_12		  = 17, 		// 12 bit Delta Width Variable Word encoding. 
		ENC_DWVW_16		  = 18, 		// 16 bit Delta Width Variable Word encoding. 
		ENC_DWVW_24		  = 19, 		// 24 bit Delta Width Variable Word encoding. 
		ENC_DWVW_N		  = 20, 		// N bit Delta Width Variable Word encoding. 

        ENC_DPCM_8		  = 21,			// 8 bit differential PCM (XI only) 
		ENC_DPCM_16		  = 22,			// 16 bit differential PCM (XI only) 

        ENC_VORBIS		  = 23,			// Xiph Vorbis encoding. 
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
            LoopNone        = 800,
            LoopForward     = 1,
            LoopBackward    = 2,
            LoopAlternating = 3
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
    virtual ~AudioFile();

    virtual void open(const Path& filename, FileOpenMode mode);
    virtual void load(AudioBuffer* buffer);
    virtual void store(const AudioBuffer* buffer);
    virtual void close() {};
	virtual int64 seek(int64 frame) = 0;
    
    virtual int64 readShort(short* buffer, int64 num)    = 0;
    virtual int64 readInt(int* buffer, int64 num)        = 0;
    virtual int64 readFloat(float* buffer, int64 num)    = 0;
    virtual int64 readDouble(double* buffer, int64 num)  = 0;

    virtual int64 writeShort(short* buffer, int64 num)   = 0;
    virtual int64 writeInt(int* buffer, int64 num)       = 0;
    virtual int64 writeFloat(float* buffer, int64 num)   = 0;
    virtual int64 writeDouble(double* buffer, int64 num) = 0;

    virtual bool isOpen() const = 0;
    virtual bool hasError() const = 0;
    virtual bool isReadable() const         { return isOpen() && (fileOpenMode_ == OpenRead  || fileOpenMode_ == OpenRdwr); }
    virtual bool isWriteable() const        { return isOpen() && (fileOpenMode_ == OpenWrite || fileOpenMode_ == OpenRdwr); }
    virtual bool checkFormat() const        { return true; }

    virtual std::string getErrorString() const = 0;
    virtual std::string getVersionString() const { return "unknown"; }
    
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

	virtual InstrumentChunk& getInstrumentChunk() { return instrumentChunk_; }

    typedef EnumNames<Format> FormatNames;
    static const FormatNames& getFormatNames()      { return formatNames_s; }

    typedef EnumNames<Encoding> EncodingNames;
    static const EncodingNames& getEncodingNames()  { return encodingNames_s; }

protected:
    virtual void loadInstrumentChunk() = 0;
    virtual void storeInstrumentChunk() = 0;

    int sampleRate_;
    int numChannels_;
	int	numSections_;
    int64 numFrames_;

    Path filename_;
    FileOpenMode fileOpenMode_;

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


