

//--------------------------------------------------------
// AudioFormat.h
//
// Base declarations for audio file formats
//--------------------------------------------------------

#pragma once

#include <vector>


namespace e3 {

    enum FormatId
    {
        FORMAT_UNKNOWN,
        FORMAT_AIFF,      // Apple/SGI AIFF format (big endian). 
        FORMAT_AU,        // Sun/NeXT AU format (big endian). 
        FORMAT_AVR,       // Audio Visual Research 
        FORMAT_CAF,       // Core Audio File format 
        FORMAT_FLAC,      // FLAC lossless file format 
        FORMAT_HTK,       // HMM Tool Kit format 
        FORMAT_IRCAM,     // Berkeley/IRCAM/CARL 
        FORMAT_MAT4,      // Matlab (tm) V4.2 / GNU Octave 2.0 
        FORMAT_MAT5,      // Matlab (tm) V5.0 / GNU Octave 2.1 
        FORMAT_MPC2K,     // Akai MPC 2000 sampler 
        FORMAT_MPEG,      // MPEG Layer I/II/III
        FORMAT_NIST,      // Sphere NIST format. 
        FORMAT_OGG,       // Xiph OGG container 
        FORMAT_PAF,       // Ensoniq PARIS file format. 
        FORMAT_PVF,       // Portable Voice Format 
        FORMAT_RAW,       // RAW PCM data. 
        FORMAT_RF64,      // RF64 WAV file 
        FORMAT_SD2,       // Sound Designer 2 
        FORMAT_SDS,       // Midi Sample Dump Standard 
        FORMAT_SVX,       // Amiga IFF / SVX8 / SV16 format. 
        FORMAT_VOC,       // VOC files. 
        FORMAT_W64,       // Sonic Foundry's 64 bit RIFF/WAV 
        FORMAT_WAV,       // Microsoft WAV format (little endian default).
        FORMAT_WAVEX,     // MS WAVE with WAVEFORMATEX 
        FORMAT_WVE,       // Psion WVE format 
        FORMAT_XI,        // Fasttracker 2 Extended Instrument 
        FORMAT_COUNT,
    };

    enum CodecId
    {
        CODEC_UNKNOWN,
        CODEC_PCM_S8,       // Signed 8 bit data 
        CODEC_PCM_S16,      // Signed 16 bit data
        CODEC_PCM_S24,      // Signed 24 bit data
        CODEC_PCM_S32,      // Signed 32 bit data
        CODEC_PCM_U8,       // Unsigned 8 bit data (WAV and RAW only) 
        CODEC_PCM_FLOAT,    // 32 bit float data 
        CODEC_PCM_DOUBLE,   // 64 bit float data 

        CODEC_ULAW,         // U-Law encoded. 
        CODEC_ALAW, 	      // A-Law encoded. 
        CODEC_IMA_ADPCM,    // IMA ADPCM. 
        CODEC_MS_ADPCM,     // Microsoft ADPCM

        CODEC_GSM610,       // GSM 6.10 encoding. 
        CODEC_VOX_ADPCM,    // OKI / Dialogix ADPCM 

        CODEC_G721_32,      // 32kbs G721 ADPCM encoding
        CODEC_G723_24,      // 24kbs G723 ADPCM encoding
        CODEC_G723_40,      // 40kbs G723 ADPCM encoding

        CODEC_DWVW_12,      // 12 bit Delta Width Variable Word encoding
        CODEC_DWVW_16,      // 16 bit Delta Width Variable Word encoding
        CODEC_DWVW_24,      // 24 bit Delta Width Variable Word encoding
        CODEC_DWVW_N,       // N bit Delta Width Variable Word encoding

        CODEC_DPCM_8,       // 8 bit differential PCM (XI only) 
        CODEC_DPCM_16,      // 16 bit differential PCM (XI only) 

        CODEC_VORBIS,       // Xiph Vorbis encoding. 
        CODEC_MP1,          // MPEG Layer I
        CODEC_MP2,          // MPEG Layer II
        CODEC_MP3,          // MPEG Layer III
        CODEC_COUNT,
    };


    struct CodecInfo
    {
        CodecInfo() :
            id_(CODEC_UNKNOWN),
            idPrivate_(0)
        {}

        CodecInfo(CodecId id, int idPrivate, const std::string& name, const std::string& description) :
            id_(id),
            idPrivate_(idPrivate),
            name_(name),
            description_(description)
        {}

        CodecId id_;
        int idPrivate_;

        std::string name_;
        std::string description_;
    };
    typedef std::vector<CodecInfo> CodecInfoVector;


    struct FormatInfo
    {
        FormatInfo() :
            id_(FORMAT_UNKNOWN),
            idPrivate_(0)
        {}

        FormatInfo(FormatId id, int idPrivate, const std::string& name, const std::string& extension, const std::string& description) :
            id_(id),
            idPrivate_(idPrivate),
            name_(name),
            extension_(extension),
            description_(description)
        {}

        FormatId id_;
        int idPrivate_;

        std::string name_;
        std::string extension_;
        std::string description_;
    };
    typedef std::vector<FormatInfo> FormatInfoVector;

} // namespace e3