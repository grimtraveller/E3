
#include <boost/assign/list_of.hpp>


#ifndef AUDIOFILE_LIB
    #define AUDIOFILE_LIB SNDFILE
#endif

#if AUDIOFILE_LIB == SNDFILE
    #include <wrapper/LibSndFile.h>
    typedef LibSndFile AudioLibBridge;
#elif AUDIOFILE_LIB == PORTSF
    #include <wrapper/PortSfFile.h>
    typedef PortSfFile AudioLibBridge;
#endif

#include <core/AudioBuffer.h>
#include <core/AudioFile.h>


//-------------------------------------------
// class AudioFile
//-------------------------------------------
    
AudioFile::AudioFile() :
    sampleRate_(44100),
    numChannels_(2),
	numSections_(1),
    numFrames_(0),
    fileOpenMode_(OpenRead),
    format_(FMT_UNKNOWN),
    encoding_(ENC_UNKNOWN),
	seekable_(false)
{}



AudioFile::~AudioFile()
{
    close();
}



void AudioFile::open(const Path& filename, FileOpenMode mode) 
{ 
    filename_     = filename;
    fileOpenMode_ = mode;
    ASSERT(filename_.empty() == false);
}



void AudioFile::load(AudioBuffer* buffer)
{
    ASSERT(isReadable());

    try {
        loadInstrumentChunk();

        buffer->setSampleRate(sampleRate_);
        buffer->setNumChannels(numChannels_);

        size_t numFloats = (size_t)(numFrames_ * numChannels_);
        buffer->resize(numFloats);

        if(buffer->getSize() == numFloats)
        {
            seek(0);
            int64 numRead = readFloat(buffer->getHead(), numFloats);

            if(numRead != numFloats) {
                EXCEPTION(std::exception, "Error reading file");
            }
        }
        else EXCEPTION(std::exception, "Not enough memory to load file");
    }
    catch(const std::exception& e) 
    {
        buffer->resize(0);
        throw e;
    }
}



void AudioFile::store(const AudioBuffer* buffer)
{
    ASSERT(buffer);
    if(buffer == NULL) 
        return;

    if(buffer->getSampleRate() != sampleRate_) 
        EXCEPTION(std::exception, "Can not store data with sample rate %d to file with sample rate %d\n", buffer->getSampleRate(), sampleRate_);

    if(isWriteable() == false)
        EXCEPTION(std::exception, "File not writeable");

    storeInstrumentChunk();        
    int64 numWritten = writeFloat(buffer->getHead(), buffer->getSize());

    if(numWritten != buffer->getSize()) {
        EXCEPTION(std::exception, "Error storing file %s", filename_.string().c_str());
    }
    numFrames_   = buffer->getNumFrames();
    numChannels_ = buffer->getNumChannels();
}


//------------------------------------------------------------
// AudioFile statics
//------------------------------------------------------------

AudioFile::FormatNames AudioFile::formatNames_s;
AudioFile::EncodingNames AudioFile::encodingNames_s;

AudioFile::Initializer AudioFile::initializer_s;


AudioFile::Initializer::Initializer()
{
    formatNames_s.add(FMT_UNKNOWN, "UNKNOWN", "Unknown");
    formatNames_s.add(FMT_WAV,     "WAV",     "Microsoft WAV");
	formatNames_s.add(FMT_AIFF,    "AIF",     "Apple/SGI AIFF");
    formatNames_s.add(FMT_AIFC,    "AIFC",    "Apple/SGI AIFF");
	formatNames_s.add(FMT_AU,	  "AU",       "Sun/NeXT AU");
	formatNames_s.add(FMT_RAW,     "RAW",     "RAW PCM data");
	formatNames_s.add(FMT_PAF,     "PAF",     "Ensoniq PARIS");
	formatNames_s.add(FMT_SVX,     "SVX",     "Amiga IFF / SVX8 / SV16");
	formatNames_s.add(FMT_NIST,    "NIST",    "Sphere NIST");
	formatNames_s.add(FMT_VOC,     "VOC",     "VOC");
	formatNames_s.add(FMT_IRCAM,   "IRCAM",   "Berkeley/IRCAM/CARL");
	formatNames_s.add(FMT_W64,     "W64",     "Sonic Foundry's 64 bit RIFF/WAV ");
	formatNames_s.add(FMT_MAT4,    "MAT4",    "Matlab V4.2 / GNU Octave 2.0");
	formatNames_s.add(FMT_MAT5,    "MAT5",    "Matlab V5.0 / GNU Octave 2.1");
	formatNames_s.add(FMT_PVF,     "PVF",     "Portable Voice Format");
	formatNames_s.add(FMT_XI,	  "XI",       "Fasttracker 2 Extended Instrument");
	formatNames_s.add(FMT_HTK,     "HTK",     "HMM Tool Kit format");
	formatNames_s.add(FMT_SDS,     "SDS",     "Midi Sample Dump Standard");
	formatNames_s.add(FMT_AVR,     "AVR",     "Audio Visual Research");
	formatNames_s.add(FMT_WAVEX,   "WAVEX",   "MS WAVE with WAVEFORMATEX");
	formatNames_s.add(FMT_SD2,     "SD2",     "Sound Designer 2");
	formatNames_s.add(FMT_FLAC,    "FLAC",    "FLAC lossless");
	formatNames_s.add(FMT_CAF,     "CAF",     "Core Audio File");
	formatNames_s.add(FMT_WVE,     "WVE",     "Psion WVE format");
	formatNames_s.add(FMT_OGG,     "OGG",     "Xiph OGG container");
	formatNames_s.add(FMT_MPC2K,   "MPC2K",   "Akai MPC 2000 sampler");
	formatNames_s.add(FMT_RF64,    "RF64",    "RF64 WAV file"); 

    encodingNames_s.add(ENC_UNKNOWN,   "UNKNOWN",   "Unknown");
    encodingNames_s.add(ENC_PCM_S8,	   "PCM_S8",    "Signed 8 bit");
	encodingNames_s.add(ENC_PCM_16,	   "PCM_16",    "Signed 16 bit");
	encodingNames_s.add(ENC_PCM_24,	   "PCM_24",    "Signed 24 bit");
	encodingNames_s.add(ENC_PCM_32,	   "PCM_32",    "Signed 32 bit");
    encodingNames_s.add(ENC_PCM_U8,	   "PCM_U8",    "Unsigned 8 bit data");
    encodingNames_s.add(ENC_FLOAT,	   "FLOAT",     "32 bit float");
	encodingNames_s.add(ENC_DOUBLE,	   "DOUBLE",    "64 bit float");
    encodingNames_s.add(ENC_ULAW,	   "ULAW",      "U-Law");
	encodingNames_s.add(ENC_ALAW,	   "ALAW",      "A-Law");
	encodingNames_s.add(ENC_IMA_ADPCM, "IMA_ADPCM", "IMA ADPCM"); 
	encodingNames_s.add(ENC_MS_ADPCM,  "MS_ADPCM",  "Microsoft ADPCM");
    encodingNames_s.add(ENC_GSM610,	   "GSM610",    "GSM 6.10");
	encodingNames_s.add(ENC_VOX_ADPCM, "VOX_ADPCM", "OKI / Dialogix ADPCM");
    encodingNames_s.add(ENC_G721_32,   "G721_32",   "32kbs G721 ADPCM");
	encodingNames_s.add(ENC_G723_24,   "G723_24",   "24kbs G723 ADPCM");
	encodingNames_s.add(ENC_G723_40,   "G723_40",   "40kbs G723 ADPCM");
    encodingNames_s.add(ENC_DWVW_12,   "DWVW_12",   "12 bit Delta Width Variable Word");
	encodingNames_s.add(ENC_DWVW_16,   "DWVW_16",   "16 bit Delta Width Variable Word");
	encodingNames_s.add(ENC_DWVW_24,   "DWVW_24",   "24 bit Delta Width Variable Word");
	encodingNames_s.add(ENC_DWVW_N,	   "DWVW_N",    "N bit Delta Width Variable Word");
    encodingNames_s.add(ENC_DPCM_8,	   "DPCM_8",    "8 bit differential PCM");
	encodingNames_s.add(ENC_DPCM_16,   "DPCM_16",   "16 bit differential PCM");
    encodingNames_s.add(ENC_VORBIS,	   "VORBIS",    "Xiph Vorbis");
}



//------------------------------------------------------------
// class AudioFile::InstrumentChunk
//------------------------------------------------------------

AudioFile::InstrumentChunk::LoopModeInfo AudioFile::InstrumentChunk::loopModeInfo_s;

AudioFile::InstrumentChunk::Initializer AudioFile::InstrumentChunk::initializer_s;


AudioFile::InstrumentChunk::Initializer::Initializer()
{
    loopModeInfo_s.add(InstrumentChunk::LoopNone,        "NONE");
    loopModeInfo_s.add(InstrumentChunk::LoopForward,     "FORWARD");
    loopModeInfo_s.add(InstrumentChunk::LoopBackward,    "BACKWARD");
    loopModeInfo_s.add(InstrumentChunk::LoopAlternating, "ALTERNATING");
}



AudioFile::InstrumentChunk::InstrumentChunk() :
    hasData_(false),
    gain_(1),
    baseNote_(60),
    keyLow_(0),
    keyHigh_(127),
    velocityLow_(0),
    velocityHigh_(127),
    detune_(0)
{}



AudioFile::InstrumentChunk::LoopData::LoopData() :
    mode_(LoopNone),
    start_(0),
    end_(0),
    numRepeats_(0)
{}


/*
AudioFile::InstrumentChunk::LoopModeInfo AudioFile::InstrumentChunk::loopModeInfo_s = boost::assign::map_list_of
    (LoopNone,         "None")
    (LoopForward,      "Forward")
    (LoopBackward,     "Backward")
    (LoopAlternating,  "Alternating");
*/





