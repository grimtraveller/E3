

//#include <stdio.h>
#include <sstream>

#include <Macros.h>
#include <wrapper/LibSndFile.h>


LibSndFile::FormatMapper   LibSndFile::formatMapper_s;
LibSndFile::EncodingMapper LibSndFile::encodingMapper_s;
LibSndFile::LoopModeMapper LibSndFile::loopModeMapper_s;

LibSndFile::Initializer LibSndFile::initializer_s;


LibSndFile::Initializer::Initializer()
{
	formatMapper_s.add( FMT_WAV,     SF_FORMAT_WAV );
	formatMapper_s.add( FMT_AIFF,    SF_FORMAT_AIFF );
    formatMapper_s.add( FMT_AIFC,    SF_FORMAT_AIFF );
	formatMapper_s.add( FMT_AU,      SF_FORMAT_AU );
	formatMapper_s.add( FMT_RAW,     SF_FORMAT_RAW );
	formatMapper_s.add( FMT_PAF,     SF_FORMAT_PAF );
	formatMapper_s.add( FMT_SVX,     SF_FORMAT_SVX );
	formatMapper_s.add( FMT_NIST,    SF_FORMAT_NIST );
	formatMapper_s.add( FMT_VOC,     SF_FORMAT_VOC );
	formatMapper_s.add( FMT_IRCAM,   SF_FORMAT_IRCAM );
	formatMapper_s.add( FMT_W64,     SF_FORMAT_W64 );
	formatMapper_s.add( FMT_MAT4,    SF_FORMAT_MAT4 );
	formatMapper_s.add( FMT_MAT5,    SF_FORMAT_MAT5 );
	formatMapper_s.add( FMT_PVF,     SF_FORMAT_PVF );
	formatMapper_s.add( FMT_XI,      SF_FORMAT_XI );
	formatMapper_s.add( FMT_HTK,     SF_FORMAT_HTK );
	formatMapper_s.add( FMT_SDS,     SF_FORMAT_SDS );
	formatMapper_s.add( FMT_AVR,     SF_FORMAT_AVR );
	formatMapper_s.add( FMT_WAVEX,   SF_FORMAT_WAVEX );
	formatMapper_s.add( FMT_SD2,     SF_FORMAT_SD2 );
	formatMapper_s.add( FMT_FLAC,    SF_FORMAT_FLAC );
	formatMapper_s.add( FMT_CAF,     SF_FORMAT_CAF );
	formatMapper_s.add( FMT_WVE,     SF_FORMAT_WVE );
	formatMapper_s.add( FMT_OGG,     SF_FORMAT_OGG );
	formatMapper_s.add( FMT_MPC2K,   SF_FORMAT_MPC2K );
	formatMapper_s.add( FMT_RF64,    SF_FORMAT_RF64 );

	encodingMapper_s.add( ENC_PCM_S8,      SF_FORMAT_PCM_S8 ); 
	encodingMapper_s.add( ENC_PCM_16,      SF_FORMAT_PCM_16 ); 
	encodingMapper_s.add( ENC_PCM_24,      SF_FORMAT_PCM_24 );
	encodingMapper_s.add( ENC_PCM_32,      SF_FORMAT_PCM_32 );
    encodingMapper_s.add( ENC_PCM_U8,      SF_FORMAT_PCM_U8 );
    encodingMapper_s.add( ENC_FLOAT,       SF_FORMAT_FLOAT );
	encodingMapper_s.add( ENC_DOUBLE,      SF_FORMAT_DOUBLE );
    encodingMapper_s.add( ENC_ULAW,        SF_FORMAT_ULAW );
	encodingMapper_s.add( ENC_ALAW, 	   SF_FORMAT_ALAW );
	encodingMapper_s.add( ENC_IMA_ADPCM,   SF_FORMAT_IMA_ADPCM );
	encodingMapper_s.add( ENC_MS_ADPCM,    SF_FORMAT_MS_ADPCM );
    encodingMapper_s.add( ENC_GSM610,      SF_FORMAT_GSM610 );
	encodingMapper_s.add( ENC_VOX_ADPCM,   SF_FORMAT_VOX_ADPCM );
    encodingMapper_s.add( ENC_G721_32,     SF_FORMAT_G721_32 ); 
	encodingMapper_s.add( ENC_G723_24,     SF_FORMAT_G723_24 ); 	
	encodingMapper_s.add( ENC_G723_40,     SF_FORMAT_G723_40 ); 	
    encodingMapper_s.add( ENC_DWVW_12,     SF_FORMAT_DWVW_12 );	
	encodingMapper_s.add( ENC_DWVW_16,     SF_FORMAT_DWVW_16 ); 	
	encodingMapper_s.add( ENC_DWVW_24,     SF_FORMAT_DWVW_24 ); 	
	encodingMapper_s.add( ENC_DWVW_N,      SF_FORMAT_DWVW_N ); 	
    encodingMapper_s.add( ENC_DPCM_8,      SF_FORMAT_DPCM_8 ); 	
	encodingMapper_s.add( ENC_DPCM_16,     SF_FORMAT_DPCM_16 );
    encodingMapper_s.add( ENC_VORBIS,      SF_FORMAT_VORBIS ); 

    loopModeMapper_s.add( InstrumentChunk::LoopNone,        SF_LOOP_NONE );
    loopModeMapper_s.add( InstrumentChunk::LoopForward,     SF_LOOP_FORWARD );
    loopModeMapper_s.add( InstrumentChunk::LoopBackward,    SF_LOOP_BACKWARD );
    loopModeMapper_s.add( InstrumentChunk::LoopAlternating, SF_LOOP_ALTERNATING );
}


LibSndFile::LibSndFile() :
    AudioFile(),
    handle_(NULL)
{}



void LibSndFile::open(const Path& filename, FileOpenMode mode)
{
    AudioFile::open(filename, mode);

    SF_INFO sfInfo;
    memset(&sfInfo, 0, sizeof(sfInfo));

    sfInfo.format     = makeSFFormat();
    sfInfo.samplerate = sampleRate_;
    sfInfo.channels   = numChannels_;

    switch(mode) 
    {
    case OpenRead:  handle_ = sf_open(filename_.string().c_str(), SFM_READ, &sfInfo);  break;
    case OpenWrite: handle_ = sf_open(filename_.string().c_str(), SFM_WRITE, &sfInfo); break;
    case OpenRdwr:  handle_ = sf_open(filename_.string().c_str(), SFM_READ, &sfInfo);  break;
    default: ASSERT(false); break;
    }

    if(handle_ != NULL)
    {
        format_      = formatMapper_s.getLeft(sfInfo.format & SF_FORMAT_TYPEMASK);
        encoding_    = encodingMapper_s.getLeft(sfInfo.format & SF_FORMAT_SUBMASK);
        sampleRate_  = sfInfo.samplerate;
        numFrames_   = sfInfo.frames;
        numChannels_ = sfInfo.channels;
        numSections_ = sfInfo.sections;
        seekable_    = sfInfo.seekable == 1;
    }
    else {
        std::ostringstream os;
        os << sf_strerror(handle_) << " (" << filename_.string().c_str() << ") ";
        EXCEPTION(std::exception, os.str().c_str());
    }
}



void LibSndFile::close()
{
    if(handle_) {
        int result = sf_close(handle_);
        handle_ = NULL;
    }
}



int64 LibSndFile::seek(int64 frame)
{
    sf_count_t pos = sf_seek(handle_, frame, SEEK_SET);

    ASSERT(pos == frame);
    return pos;
}



bool LibSndFile::checkFormat() const
{
    SF_INFO sfInfo;
    memset(&sfInfo, 0, sizeof(sfInfo));

    sfInfo.format     = makeSFFormat();
    sfInfo.samplerate = sampleRate_;
    sfInfo.channels   = numChannels_;

    int result = sf_format_check(&sfInfo);
    return result != SF_FALSE;
}



std::string LibSndFile::getVersionString() const
{
	static char buffer[256] ;
	sf_command(NULL, SFC_GET_LIB_VERSION, buffer, sizeof(buffer)) ;
	return buffer;
}



void LibSndFile::loadInstrumentChunk()
{
    if(handle_ == NULL)
        return;

    SF_INSTRUMENT data;
    int result = sf_command(handle_, SFC_GET_INSTRUMENT, &data, sizeof(data));

    if(result != SF_FALSE)
    {
        instrumentChunk_.setGain(data.gain);
        instrumentChunk_.setBaseNote(data.basenote);
        instrumentChunk_.setKeyLow(data.key_lo);
        instrumentChunk_.setKeyHigh(data.key_hi);
        instrumentChunk_.setVelocityLow(data.velocity_lo);
        instrumentChunk_.setVelocityHigh(data.velocity_hi);
        instrumentChunk_.setDetune(data.detune);

        instrumentChunk_.clearLoops();
        for(int i=0; i<data.loop_count; ++i)
        {
            InstrumentChunk::LoopData loopData;

            loopData.mode_       = loopModeMapper_s.getLeft(data.loops[i].mode);
            loopData.start_      = data.loops[i].start;
            loopData.end_        = data.loops[i].end;
            loopData.numRepeats_ = data.loops[i].count;

            instrumentChunk_.addLoop(loopData);
        }
    }
};



void LibSndFile::storeInstrumentChunk()
{
    if(handle_== NULL || instrumentChunk_.hasData() == false)
        return;

    SF_INSTRUMENT data;

    data.gain        = instrumentChunk_.getGain();
    data.basenote    = instrumentChunk_.getBaseNote();
    data.key_lo      = instrumentChunk_.getKeyLow();
    data.key_hi      = instrumentChunk_.getKeyHigh();
    data.velocity_lo = instrumentChunk_.getVelocityLow();
    data.velocity_hi = instrumentChunk_.getVelocityHigh();
    data.detune      = instrumentChunk_.getDetune();
    data.loop_count  = instrumentChunk_.getNumLoops();

    const InstrumentChunk::LoopVector& loops = instrumentChunk_.getLoops();
    for(unsigned i=0; i<loops.size(); ++i)
    {
        const InstrumentChunk::LoopData& loop = loops[i];

        data.loops[i].mode   = loopModeMapper_s.getRight(loop.mode_);
        data.loops[i].start  = loop.start_;
        data.loops[i].end    = loop.end_;
        data.loops[i].count  = loop.numRepeats_;
    }
    int result = sf_command(handle_, SFC_SET_INSTRUMENT, &data, sizeof(data));
}


int LibSndFile::makeSFFormat() const                        
{ 
    try {
        return formatMapper_s.getRight(format_) | encodingMapper_s.getRight(encoding_); 
    }
    catch(const EnumException&) {
        return 0;
    }
}
