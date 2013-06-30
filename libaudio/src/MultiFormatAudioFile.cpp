
#include <sstream>
#include <boost/assign/list_of.hpp>

#include <AudioBuffer.h>
#include <InstrumentChunk.h>
#include <MultiFormatAudioFile.h>
#include <FormatManager.h>


//-------------------------------------------
// class MultiFormatAudioFile
//-------------------------------------------
    
MultiFormatAudioFile::MultiFormatAudioFile() : AudioFile(),
	numSections_(1),
    handle_(NULL),
	seekable_(false)
{}



MultiFormatAudioFile::~MultiFormatAudioFile()
{
    close();

    if(instrumentChunk_ != NULL) {
        delete instrumentChunk_;
        instrumentChunk_ = NULL;
    }
}



void MultiFormatAudioFile::open(const Path& filename, FileOpenMode mode) 
{ 
    AudioFile::open(filename, mode);

    SF_INFO sfInfo;
    memset(&sfInfo, 0, sizeof(sfInfo));

    sfInfo.format     = makeSfFormat();
    sfInfo.samplerate = sampleRate_;
    sfInfo.channels   = numChannels_;

    switch(fileOpenMode_) 
    {
    case OpenRead:  handle_ = sf_open(filename_.string().c_str(), SFM_READ,  &sfInfo); break;
    case OpenWrite: handle_ = sf_open(filename_.string().c_str(), SFM_WRITE, &sfInfo); break;
    case OpenRdwr:  handle_ = sf_open(filename_.string().c_str(), SFM_READ,  &sfInfo); break;
    default: ASSERT(false); break;
    }

    if(handle_ != NULL)
    {
        format_      = FormatManager::getFormat(sfInfo.format & SF_FORMAT_TYPEMASK);
        codec_       = FormatManager::getCodec(sfInfo.format & SF_FORMAT_SUBMASK);
        sampleRate_  = sfInfo.samplerate;
        numFrames_   = sfInfo.frames;
        numChannels_ = sfInfo.channels;
        numSections_ = sfInfo.sections;
        seekable_    = sfInfo.seekable == 1;
    }

    if(sf_error(handle_) != SF_ERR_NO_ERROR) 
    {
        std::ostringstream os;
        os << sf_strerror(handle_) << " (" << filename_.string().c_str() << ") ";
        EXCEPTION(std::exception, os.str().c_str());
    }
}



void MultiFormatAudioFile::close()
{
    if(handle_) {
        int result = sf_close(handle_);
        handle_ = NULL;
    }
}



void MultiFormatAudioFile::load(AudioBuffer* buffer)
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

            if(sf_error(handle_) != SF_ERR_NO_ERROR) {
                EXCEPTION(std::exception, sf_strerror(handle_));
            }
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



void MultiFormatAudioFile::store(const AudioBuffer* buffer)
{
    ASSERT(buffer);
    if(buffer == NULL) 
        return;

    if(buffer->getSampleRate() != sampleRate_) 
        EXCEPTION(std::exception, "Can not store data with sample rate %d to file with sample rate %d", buffer->getSampleRate(), sampleRate_);

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



int64 MultiFormatAudioFile::seek(int64 frame)
{
    sf_count_t pos = sf_seek(handle_, frame, SEEK_SET);

    //ASSERT(pos == frame);
    return pos;
}


void MultiFormatAudioFile::loadInstrumentChunk()
{
    if(handle_ == NULL)
        return;

    SF_INSTRUMENT data;
    int result = sf_command(handle_, SFC_GET_INSTRUMENT, &data, sizeof(data));

    if(result != SF_FALSE)
    {
        if(instrumentChunk_ != NULL)
            delete instrumentChunk_;
        instrumentChunk_ = new InstrumentChunk();

        instrumentChunk_->setGain(data.gain);
        instrumentChunk_->setBaseNote(data.basenote);
        instrumentChunk_->setKeyLow(data.key_lo);
        instrumentChunk_->setKeyHigh(data.key_hi);
        instrumentChunk_->setVelocityLow(data.velocity_lo);
        instrumentChunk_->setVelocityHigh(data.velocity_hi);
        instrumentChunk_->setDetune(data.detune);
                        
        instrumentChunk_->clearLoops();
        for(int i=0; i<data.loop_count; ++i)
        {
            InstrumentChunk::LoopData loopData;

            switch(data.loops[i].mode) {
            case SF_LOOP_NONE:        loopData.mode_ = InstrumentChunk::LoopNone; break;
            case SF_LOOP_FORWARD:     loopData.mode_ = InstrumentChunk::LoopForward; break;
            case SF_LOOP_BACKWARD:    loopData.mode_ = InstrumentChunk::LoopBackward; break;
            case SF_LOOP_ALTERNATING: loopData.mode_ = InstrumentChunk::LoopAlternating; break;
            default:                  loopData.mode_ = InstrumentChunk::LoopNone;
            }
            loopData.start_      = data.loops[i].start;
            loopData.end_        = data.loops[i].end;
            loopData.numRepeats_ = data.loops[i].count;

            instrumentChunk_->addLoop(loopData);
        }
    }
}



void MultiFormatAudioFile::storeInstrumentChunk()
{
    if(handle_== NULL || instrumentChunk_ == NULL)
        return;

    SF_INSTRUMENT data;

    data.gain        = instrumentChunk_->getGain();
    data.basenote    = instrumentChunk_->getBaseNote();
    data.key_lo      = instrumentChunk_->getKeyLow();
    data.key_hi      = instrumentChunk_->getKeyHigh();
    data.velocity_lo = instrumentChunk_->getVelocityLow();
    data.velocity_hi = instrumentChunk_->getVelocityHigh();
    data.detune      = instrumentChunk_->getDetune();
    data.loop_count  = instrumentChunk_->getNumLoops();

    const InstrumentChunk::LoopVector& loops = instrumentChunk_->getLoops();
    for(unsigned i=0; i<loops.size(); ++i)
    {
        const InstrumentChunk::LoopData& loop = loops[i];

        switch(loop.mode_) {
        case InstrumentChunk::LoopNone:        data.loops[i].mode = SF_LOOP_NONE; break;
        case InstrumentChunk::LoopForward:     data.loops[i].mode = SF_LOOP_FORWARD; break;
        case InstrumentChunk::LoopBackward:    data.loops[i].mode = SF_LOOP_BACKWARD; break;
        case InstrumentChunk::LoopAlternating: data.loops[i].mode = SF_LOOP_ALTERNATING; break;
        default:              data.loops[i].mode = SF_LOOP_NONE;  
        }
        data.loops[i].start = loop.start_;
        data.loops[i].end   = loop.end_;
        data.loops[i].count = loop.numRepeats_;
    }
    int result = sf_command(handle_, SFC_SET_INSTRUMENT, &data, sizeof(data));
}



//------------------------------------------------------------
// MultiFormatAudioFile statics
//------------------------------------------------------------


std::string MultiFormatAudioFile::getVersionString()
{
	static char buffer[128] ;
	sf_command(NULL, SFC_GET_LIB_VERSION, buffer, sizeof(buffer)) ;
	return buffer;
}



bool MultiFormatAudioFile::isFormatSupported(const FormatInfo& format, const CodecInfo& codec, int sampleRate, int numChannels)
{
    SF_INFO sfInfo;
    memset(&sfInfo, 0, sizeof(sfInfo));

    sfInfo.format     = (format.idPrivate_ & SF_FORMAT_TYPEMASK) | codec.idPrivate_;
    sfInfo.channels   = numChannels;
    sfInfo.samplerate = sampleRate;

    int result = sf_format_check(&sfInfo);
    return result != SF_FALSE;

}



void MultiFormatAudioFile::initFormatInfos(FormatInfoVector& infos)
{
    SF_FORMAT_INFO sfInfo;
    int count;
    sf_command (NULL, SFC_GET_FORMAT_MAJOR_COUNT, &count, sizeof (int));

    for(int i=0; i<count; i++)
    {
        sfInfo.format = i;
        sf_command(NULL, SFC_GET_FORMAT_MAJOR, &sfInfo, sizeof(sfInfo));

        switch(sfInfo.format)
        {
        case SF_FORMAT_WAV:	
            infos.push_back(FormatInfo(FORMAT_WAV, sfInfo.format, "WAV", sfInfo.extension, "MICROSOFT WAV")); break;
        case SF_FORMAT_AIFF:	
            infos.push_back(FormatInfo(FORMAT_AIFF, sfInfo.format, "AIFF", sfInfo.extension, "Apple/SGI AIFF")); break;
        case SF_FORMAT_AU:
            infos.push_back(FormatInfo(FORMAT_AU, sfInfo.format, "AU", sfInfo.extension, "Sun/NeXT AU")); break;
        case SF_FORMAT_RAW:	
            infos.push_back(FormatInfo(FORMAT_RAW, sfInfo.format, "RAW", sfInfo.extension, "RAW PCM data")); break;
        case SF_FORMAT_PAF:	
            infos.push_back(FormatInfo(FORMAT_PAF, sfInfo.format, "PAF", sfInfo.extension, "Ensoniq PARIS file format")); break;
        case SF_FORMAT_SVX:	
            infos.push_back(FormatInfo(FORMAT_SVX, sfInfo.format, "SVX", sfInfo.extension, "Amiga IFF/SVX8/SV16")); break;
        case SF_FORMAT_NIST:	
            infos.push_back(FormatInfo(FORMAT_NIST, sfInfo.format, "NIST", sfInfo.extension, "Sphere NIST format")); break;
        case SF_FORMAT_VOC:	
            infos.push_back(FormatInfo(FORMAT_VOC, sfInfo.format, "VOC", sfInfo.extension, "VOC files")); break;
        case SF_FORMAT_IRCAM:	
            infos.push_back(FormatInfo(FORMAT_IRCAM, sfInfo.format, "IRCAM", sfInfo.extension, "Berkeley/IRCAM/CARL")); break;
        case SF_FORMAT_W64:	
            infos.push_back(FormatInfo(FORMAT_W64, sfInfo.format, "W64", sfInfo.extension, "Sonic Foundry 64 bit RIFF/WAV")); break;
        case SF_FORMAT_MAT4:	
            infos.push_back(FormatInfo(FORMAT_MAT4, sfInfo.format, "MAT4", sfInfo.extension, "Matlab V4.2 / GNU Octave 2.0")); break;
        case SF_FORMAT_MAT5:	
            infos.push_back(FormatInfo(FORMAT_MAT5, sfInfo.format, "MAT5", sfInfo.extension, "Matlab (tm) V5.0 / GNU Octave 2.1")); break;
        case SF_FORMAT_PVF:	
            infos.push_back(FormatInfo(FORMAT_PVF, sfInfo.format, "PVF", sfInfo.extension, "Portable Voice Format")); break;
        case SF_FORMAT_XI:	
            infos.push_back(FormatInfo(FORMAT_XI, sfInfo.format, "XI", sfInfo.extension, "Fasttracker 2 Extended Instrument")); break;
        case SF_FORMAT_HTK:	
            infos.push_back(FormatInfo(FORMAT_HTK, sfInfo.format, "HTK", sfInfo.extension, "HMM Tool Kit format")); break;
        case SF_FORMAT_SDS:	
            infos.push_back(FormatInfo(FORMAT_SDS, sfInfo.format, "SDS", sfInfo.extension, "Midi Sample Dump Standard")); break;
        case SF_FORMAT_AVR:	
            infos.push_back(FormatInfo(FORMAT_AVR, sfInfo.format, "AVR", sfInfo.extension, "Audio Visual Research")); break;
        case SF_FORMAT_WAVEX:	
            infos.push_back(FormatInfo(FORMAT_WAVEX, sfInfo.format, "WAVEX", sfInfo.extension, "Microsoft WAVE with WAVEFORMATEX")); break;
        case SF_FORMAT_SD2:	
            infos.push_back(FormatInfo(FORMAT_SD2, sfInfo.format, "SD2", sfInfo.extension, "Sound Designer 2")); break;
        case SF_FORMAT_FLAC:	
            infos.push_back(FormatInfo(FORMAT_FLAC, sfInfo.format, "FLAC", sfInfo.extension, "FLAC lossless file format")); break;
        case SF_FORMAT_CAF:	
            infos.push_back(FormatInfo(FORMAT_CAF, sfInfo.format, "CAF", sfInfo.extension, "Apple Core Audio File format")); break;
        case SF_FORMAT_WVE:	
            infos.push_back(FormatInfo(FORMAT_WVE, sfInfo.format, "WVE", sfInfo.extension, "Psion WVE format")); break;
        case SF_FORMAT_OGG:	
            infos.push_back(FormatInfo(FORMAT_OGG, sfInfo.format, "OGG", sfInfo.extension, "Xiph OGG container")); break;
        case SF_FORMAT_MPC2K:
            infos.push_back(FormatInfo(FORMAT_MPC2K, sfInfo.format, "MPC2K", sfInfo.extension, "Akai MPC 2000 sampler")); break;
        case SF_FORMAT_RF64:	
            infos.push_back(FormatInfo(FORMAT_RF64, sfInfo.format, "RF64", sfInfo.extension, "RF64 WAV file")); break;

        default: EXCEPTION(std::exception, "Illegal LibSndFile format %d", sfInfo.format);
        } // switch sfInfo.format
    }
}



void MultiFormatAudioFile::initCodecInfos(CodecInfoVector& infos)
{
    SF_FORMAT_INFO sfInfo;
    int count;
    sf_command (NULL, SFC_GET_FORMAT_SUBTYPE_COUNT, &count, sizeof (int));
    
    for(int i=0; i<count; i++)
    {
        sfInfo.format = i;
        sf_command(NULL, SFC_GET_FORMAT_SUBTYPE, &sfInfo, sizeof(sfInfo));

        switch(sfInfo.format)
        {
	    case SF_FORMAT_PCM_S8:
            infos.push_back(CodecInfo(CODEC_PCM_S8, sfInfo.format, "PCM_S8", "Signed 8 bit PCM")); break;
	    case SF_FORMAT_PCM_16:
            infos.push_back(CodecInfo(CODEC_PCM_S16, sfInfo.format, "PCM_S16", "Signed 16 bit PCM")); break;
	    case SF_FORMAT_PCM_24:
            infos.push_back(CodecInfo(CODEC_PCM_S24, sfInfo.format, "PCM_S24", "Signed 24 bit PCM")); break;
	    case SF_FORMAT_PCM_32:
            infos.push_back(CodecInfo(CODEC_PCM_S32, sfInfo.format, "PCM_S32", "Signed 32 bit PCM")); break;
	    case SF_FORMAT_PCM_U8:
            infos.push_back(CodecInfo(CODEC_PCM_U8, sfInfo.format, "PCM_U8", "Unsigned 8 bit PCM")); break;
	    case SF_FORMAT_FLOAT:
            infos.push_back(CodecInfo(CODEC_PCM_FLOAT, sfInfo.format, "PCM_FLOAT", "32 bit float PCM")); break;
	    case SF_FORMAT_DOUBLE:
            infos.push_back(CodecInfo(CODEC_PCM_DOUBLE, sfInfo.format, "PCM_DOUBLE", "64 bit float PCM")); break;
	    case SF_FORMAT_ULAW:
            infos.push_back(CodecInfo(CODEC_ULAW, sfInfo.format, "ULAW", "U-Law encoded")); break;
	    case SF_FORMAT_ALAW:
            infos.push_back(CodecInfo(CODEC_ALAW, sfInfo.format, "ALAW", "A-Law encoded")); break;
	    case SF_FORMAT_IMA_ADPCM:
            infos.push_back(CodecInfo(CODEC_IMA_ADPCM, sfInfo.format, "IMA_ADPCM", "IMA ADPCM")); break;
	    case SF_FORMAT_MS_ADPCM:
            infos.push_back(CodecInfo(CODEC_MS_ADPCM, sfInfo.format, "MS_ADPCM", "Microsoft ADPCM")); break;
	    case SF_FORMAT_GSM610:
            infos.push_back(CodecInfo(CODEC_GSM610, sfInfo.format, "GSM610", "GSM 6.10 encoding")); break;
	    case SF_FORMAT_VOX_ADPCM:
            infos.push_back(CodecInfo(CODEC_VOX_ADPCM, sfInfo.format, "VOX_ADPCM", "OKI/Dialogix ADPCM")); break;
	    case SF_FORMAT_G721_32:
            infos.push_back(CodecInfo(CODEC_G721_32, sfInfo.format, "G721_32", "32kbs G721 ADPCM encoding")); break;
	    case SF_FORMAT_G723_24:
            infos.push_back(CodecInfo(CODEC_G723_24, sfInfo.format, "G723_24", "24kbs G723 ADPCM encoding")); break;
	    case SF_FORMAT_G723_40:
            infos.push_back(CodecInfo(CODEC_G723_40, sfInfo.format, "G723_40", "40kbs G723 ADPCM encoding")); break;
	    case SF_FORMAT_DWVW_12:
            infos.push_back(CodecInfo(CODEC_DWVW_12, sfInfo.format, "DWVW_12", "12 bit Delta Width Variable Word encoding")); break;
	    case SF_FORMAT_DWVW_16:
            infos.push_back(CodecInfo(CODEC_DWVW_16, sfInfo.format, "DWVW_16", "16 bit Delta Width Variable Word encoding")); break;
	    case SF_FORMAT_DWVW_24:
            infos.push_back(CodecInfo(CODEC_DWVW_24, sfInfo.format, "DWVW_24", "24 bit Delta Width Variable Word encoding")); break;
	    case SF_FORMAT_DWVW_N:
            infos.push_back(CodecInfo(CODEC_DWVW_N, sfInfo.format, "DWVW_N", "N bit Delta Width Variable Word encoding")); break;
	    case SF_FORMAT_DPCM_8:
            infos.push_back(CodecInfo(CODEC_DPCM_8, sfInfo.format, "DPCM_8", "8 bit differential PCM")); break;
        case SF_FORMAT_DPCM_16:
            infos.push_back(CodecInfo(CODEC_DPCM_16, sfInfo.format, "DPCM_16", "16 bit differential PCM")); break;
        case SF_FORMAT_VORBIS:
            infos.push_back(CodecInfo(CODEC_VORBIS, sfInfo.format, "VORBIS", "Xiph Vorbis encoding")); break;

        default: EXCEPTION(std::exception, "Illegal LibSndFile codec %d", sfInfo.format);
        } // switch sfInfo.format
    }
}
