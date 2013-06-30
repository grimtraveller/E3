
//--------------------------------------------------------
// AudioFormatManager.cpp
//--------------------------------------------------------

#include <cstdio>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#ifdef __unix__
    #include <unistd.h>
#else
    #include <io.h>
#endif
#include <sys/stat.h>

#ifdef USING_ID3TAG
  #include <id3tag.h>
#if defined(HAVE_UNISTD_H)
  #include <unistd.h>
#elif defined(HAVE_IO_H)
  #include <io.h>
#endif
#else
  #define ID3_TAG_FLAG_FOOTERPRESENT 0x10
#endif

#include <algorithm>

#include <IntegerTypes.h>
#include <AudioBuffer.h>
#include <AudioFormat.h>
#include <FormatManager.h>
#include <MpegFile.h>



MpegFile::MpegFile() : AudioFile(),
    handle_(NULL),
    currentSample_(-1),
    signalLength_(0)
{
}


MpegFile::~MpegFile()
{
    close();
}



void MpegFile::open(const Path& filename, FileOpenMode mode) 
{ 
    AudioFile::open(filename, mode);

    switch(fileOpenMode_) 
    {
    case OpenRead: handle_ = fopen(filename_.string().c_str(), "rb"); break;
    default:       EXCEPTION(std::exception, "Only read mode suported for MPEG");
    }

    if(handle_ == NULL) {
        EXCEPTION(std::exception, "%s: %s", strerror(errno), filename_.string().c_str());
    }

    mpegBuffer_.clear();
    mpegBuffer_.resize(8192);

    if(isSeekable()) {
        signalLength_ = getDuration();
    }

    mad_stream_init(&madStream_);
    mad_frame_init(&madFrame_);
    mad_synth_init(&madSynth_);
    mad_timer_reset(&madTimer_);

    unsigned char* buffer = mpegBuffer_.getHead();
    size_t bufferSize     = mpegBuffer_.getSize();

    // Decode at least one valid frame to find out the input format.
    // The decoded frame will be saved off so that it can be processed later.
    //
    size_t bytesRead = fread(buffer, (size_t)1, bufferSize, handle_);
    if (bytesRead != bufferSize && ferror(handle_))
        EXCEPTION(std::exception, "%s: %s", strerror(errno), filename_.string().c_str());
    mad_stream_buffer(&madStream_, buffer, bytesRead);

    // Find a valid frame before starting up.
    // This make sure that we have a valid MP3 
    // and also skips past ID3v2 tags at the beginning of the audio file.
    //
    madStream_.error = MAD_ERROR_NONE;
    while(mad_frame_decode(&madFrame_, &madStream_))
    {
        // check whether input buffer needs a refill 
        if (madStream_.error == MAD_ERROR_BUFLEN) {
            fileRead();
            continue;
        }
        consumeId3Tag();    // consume any ID3 tags

        // FIXME: We should probably detect when we've read
        // a bunch of non-ID3 data and still haven't found a
        // frame.  In that case we can abort early without
        // scanning the whole file.
        //
        madStream_.error = MAD_ERROR_NONE;
    }

    if (madStream_.error) {
        EXCEPTION(std::exception, "No valid MP3 frame found");
    }
    
    switch(madFrame_.header.mode)
    {
    case MAD_MODE_SINGLE_CHANNEL:
    case MAD_MODE_DUAL_CHANNEL:
    case MAD_MODE_JOINT_STEREO:
    case MAD_MODE_STEREO:
        numChannels_ = madFrame_.header.mode ? 2 : 1;
        break;
    default:
        EXCEPTION(std::exception, "Cannot determine number of channels");
    }

    mad_timer_add(&madTimer_, madFrame_.header.duration);
    mad_synth_frame(&madSynth_, &madFrame_);

    unsigned int precision_ = 16;
    sampleRate_ = madSynth_.pcm.samplerate;
    //numFrames_  = 1; // TODO: use madSynth_.pcm.length

    //ft->signal.length = (uint64_t)(ft->signal.length * .001 * ft->signal.rate + .5);
    //ft->signal.length *= ft->signal.channels;  // Keep separate from line above! 

    currentSample_ = 0;
    format_        = FormatManager::getFormat(FORMAT_MPEG);
    codec_         = FormatManager::getCodec(CODEC_MPEG);
    //return SOX_SUCCESS;
}



void MpegFile::load(AudioBuffer* buffer)
{
    ASSERT(isReadable());

    try {
        buffer->setSampleRate(sampleRate_);
        buffer->setNumChannels(numChannels_);

        //size_t numFloats = (madSynth_.pcm.length) * numChannels_;
        //size_t numFloats = (size_t)(signalLength_ * .001 * sampleRate_ + .5);
        numFrames_ = (size_t)(signalLength_ * .001 * sampleRate_ + .5);
        size_t numFloats = numFrames_ * numChannels_;
        //size_t numFloats = signalLength_ * sizeof(float);
        buffer->resize(numFloats);

        if(buffer->getSize() == numFloats)
        {
            int read = mpegRead(numFloats, buffer);
        }
    }
    catch(const std::exception& e) 
    {
        buffer->resize(0);
        throw e;
    }
}



void MpegFile::close()
{
    if(handle_) {
        fclose(handle_);
        handle_ = NULL;
    }
}


//-----------------------------------------------------------
// Protected members
//-----------------------------------------------------------

int64 MpegFile::getDuration()
{
    struct mad_stream madStream;
    struct mad_frame  madFrame;
    struct mad_header madHeader;
    mad_timer_t  time = mad_timer_zero;

    bool depadded = false;
    bool vbr = false;
    size_t tagsize = 0;
    size_t consumed = 0; 
    size_t frames = 0;
    size_t initialBitrate = 0; 


    mad_stream_init(&madStream);
    mad_header_init(&madHeader);
    mad_frame_init(&madFrame);

    unsigned char* buffer = mpegBuffer_.getHead();
    size_t bufferSize     = mpegBuffer_.getSize();

    do  // Read data from the MP3 file 
    {
        int padding = 0;
        size_t leftover = madStream.bufend - madStream.next_frame;
        memcpy(buffer, madStream.this_frame, leftover);
        
        int bytesRead = fread(buffer + leftover, (size_t)1, bufferSize - leftover, handle_);
        if(bytesRead <= 0) {
            break;
        }
        for (; !depadded && padding < bytesRead && !buffer[padding]; ++padding);
        depadded = true;
        mad_stream_buffer(&madStream, buffer + padding, leftover + bytesRead - padding);

        while(true)   // decode frame headers 
        {
            madStream.error = MAD_ERROR_NONE;
            if(mad_header_decode(&madHeader, &madStream) == -1) 
            {
                if(madStream.error == MAD_ERROR_BUFLEN)     // Normal behaviour; get some more data from the file
                    break;  
                if(MAD_RECOVERABLE(madStream.error) == 0)
                    break;
                if(madStream.error == MAD_ERROR_LOSTSYNC) 
                {
                    unsigned available = (madStream.bufend - madStream.this_frame);
                    tagsize = getId3TagSize(madStream.this_frame, (size_t)available);

                    if(tagsize)    // It's some ID3 tags, so just skip 
                    {
                        if(tagsize >= available) {
                            _fseeki64(handle_, (int64)(tagsize - available), SEEK_CUR);
                            depadded = false;
                        }
                        mad_stream_skip(&madStream, std::min(tagsize, available));
                    }
                }
                continue; // not an audio frame
            } 

            mad_timer_add(&time, madHeader.duration);
            consumed += madStream.next_frame - madStream.this_frame;

            if (!frames) {
                initialBitrate = madHeader.bitrate;

                // Get the precise frame count from the XING header if present 
                madFrame.header = madHeader;
                if (mad_frame_decode(&madFrame, &madStream) == -1)
                {
                    if(MAD_RECOVERABLE(madStream.error) == 0) {
                        break;
                    }
                }
                if ((frames = xingFrames(madStream.anc_ptr, madStream.anc_bitlen))) 
                {
                    mad_timer_multiply(&time, (signed long)frames);
                    break;
                }
            }
            else {
                vbr |= madHeader.bitrate != initialBitrate;
            }

            // If not VBR, we can time just a few frames then extrapolate 
            if (++frames == 25 && !vbr) 
            {
                struct stat st;
                fstat(fileno(handle_), &st);
                madTimerMultiply(&time, (double)(st.st_size - tagsize) / consumed);
                break;
            }
        }   // while(true)
    }
    while (madStream.error == MAD_ERROR_BUFLEN);

    mad_frame_finish(&madFrame);
    mad_header_finish(&madHeader);
    mad_stream_finish(&madStream);
    rewind(handle_);

    return mad_timer_count(time, MAD_UNITS_MILLISECONDS);
}



bool MpegFile::isSeekable() const
{
  if(handle_ == NULL)
      return false;

  struct stat st;
  fstat(fileno(handle_), &st);

  return ((st.st_mode & S_IFMT) == S_IFREG);
}


//---------------------------------------------------------------------
// Read from the mpeg file and (re)fill the stream buffer that is to be decoded.  
// If any data still exists in the buffer then they are first shifted to be
// front of the stream buffer.
//
void MpegFile::fileRead()
{
    // libmad does not consume all the buffer it's given. Some
    // data, part of a truncated frame, is left unused at the
    // end of the buffer. That data must be put back at the
    // beginning of the buffer and taken in account for
    // refilling the buffer. This means that the input buffer
    // must be large enough to hold a complete frame at the
    // highest observable bit-rate (currently 448 kb/s).
    // TODO: Is 2016 bytes the size of the largest frame?
    // (448000*(1152/32000))/8
    //
    size_t leftover = madStream_.bufend - madStream_.next_frame;
    memmove(mpegBuffer_.getHead(), madStream_.next_frame, leftover);

    size_t bytesRead = fread(mpegBuffer_.getHead()+leftover, (size_t)1, mpegBuffer_.getSize()-leftover, handle_);

    if (bytesRead != mpegBuffer_.getSize() && ferror(handle_))
        EXCEPTION(std::exception, "%s: %s", strerror(errno), filename_.string().c_str());
    if (bytesRead == 0)
        EXCEPTION(std::exception, "Error reading %s", filename_.string().c_str());

    mad_stream_buffer(&madStream_, mpegBuffer_.getHead(), bytesRead+leftover);
    madStream_.error = MAD_ERROR_NONE;
}


//
// Read up samples from madSynth_
// If needed, read some more MP3 data, decode them and synth them
// Place in audioBuffer_.
// Return number of samples read.
//
size_t MpegFile::mpegRead(size_t samplesToRead, AudioBuffer* buffer)
{
    size_t readNow, samplesRead;
    size_t samplesReadTotal = 0;
    size_t done=0;
    mad_fixed_t sample;
    int channel;

    do {
        size_t unread = (madSynth_.pcm.length - currentSample_) * numChannels_;
        readNow = std::min(samplesToRead, unread);
        samplesRead = 0;

        while(samplesRead < readNow)
        {
            for(channel=0; channel<numChannels_; channel++)
            {
                sample = madSynth_.pcm.samples[channel][currentSample_];
                if (sample < -MAD_F_ONE)
                    sample = -MAD_F_ONE;
                else if (sample >= MAD_F_ONE)
                    sample = MAD_F_ONE-1;

                int iTest = (int)(sample<<(32-1-MAD_F_FRACBITS));
                float fTest = (float) (sample / (float) (1L << MAD_F_FRACBITS));

                //int64 offset = buffer->getPos();
                float* pos = buffer->getHead() + samplesReadTotal;
                *pos = fTest;
                //buffer->advance(1);
                //*buf++=(int)(sample<<(32-1-MAD_F_FRACBITS));
                samplesRead++;
                samplesReadTotal++;
            }
            currentSample_++;
        }

        samplesToRead -= readNow;
        done += readNow;

        if (samplesToRead == 0) break;

        // check whether input buffer needs a refill 
        if (madStream_.error == MAD_ERROR_BUFLEN) {
            fileRead();
        }

        if (mad_frame_decode(&madFrame_, &madStream_))
        {
            if(MAD_RECOVERABLE(madStream_.error)) {
                consumeId3Tag();
                continue;
            }
            else  {
                if (madStream_.error == MAD_ERROR_BUFLEN)
                    continue;
                else 
                    EXCEPTION(std::exception, "unrecoverable frame level error (%s).", mad_stream_errorstr(&madStream_));
            }
        }
        //numFrames_++;
        mad_timer_add(&madTimer_, madFrame_.header.duration);
        mad_synth_frame(&madSynth_, &madFrame_);
        currentSample_ = 0;
    } 
    while(true);

    return done;
}



// Attempts to read an ID3 tag at the current location in stream and
// consume it all.  Returns SOX_EOF if no tag is found.  Its up to
// caller to recover.
// Returns true if a tag was found and consumed.
//
bool MpegFile::consumeId3Tag()
{
    // FIXME: This needs some more work if we are to ever
    // look at the ID3 frame.  This is because the Stream
    // may not be able to hold the complete ID3 frame.
    // We should consume the whole frame inside getId3TagSize()
    // instead of outside of tagframe().  That would support
    // recovering when Stream contains less then 8-bytes (header)
    // and also when ID3v2 is bigger then Stream buffer size.
    // Need to pass in stream so that buffer can be
    // consumed as well as letting additional data to be
    // read in.
    //
    bool result = false;
    
    size_t leftover = madStream_.bufend - madStream_.next_frame;
    size_t tagsize  = getId3TagSize(madStream_.this_frame, leftover);
    if(tagsize > 0)
    {
        mad_stream_skip(&madStream_, tagsize);
        result = true;
    }
    /* We know that a valid frame hasn't been found yet
     * so help libmad out and go back into frame seek mode.
     * This is true whether an ID3 tag was found or not.
     */
    mad_stream_sync(&madStream_);

    return result;
}






//-----------------------------------------------------------
// Static members
//-----------------------------------------------------------

bool MpegFile::isFormatSupported(const FormatInfo& format, const CodecInfo& codec, int sampleRate, int numChannels)
{
    return format.id_ == FORMAT_MPEG && codec.id_ == CODEC_MPEG;
}



void MpegFile::initFormatInfos(FormatInfoVector& infos)
{
    infos.push_back(FormatInfo(FORMAT_MPEG, -1, "MPEG", "mp3", "MPEG Layer 1/2/3 lossy audio compression"));
}



void MpegFile::initCodecInfos(CodecInfoVector& infos)
{
    infos.push_back(CodecInfo(CODEC_MPEG, -1, "MPEG", "MPEG Layer 1/2/3"));
}



//-------------------------------------------------------------------
// Merges the functions tagtype() and id3_tag_query()
// from MAD's libid3tag, so we don't have to link to it
// Returns 0 if the frame is not an ID3 tag, tag length if it is
//-------------------------------------------------------------------

int MpegFile::getId3TagSize(const unsigned char* data, size_t length)
{
    // ID3V1
    if (length >= 3 && data[0] == 'T' && data[1] == 'A' && data[2] == 'G')
    {
        return 128; 
    }

    // ID3V2
    if (length >= 10 &&
        (data[0] == 'I' && data[1] == 'D' && data[2] == '3') &&
        data[3] < 0xff && data[4] < 0xff &&
        data[6] < 0x80 && data[7] < 0x80 && data[8] < 0x80 && data[9] < 0x80)
    {     
        unsigned char flags;
        unsigned int size;
        flags = data[5];
        size = 10 + (data[6]<<21) + (data[7]<<14) + (data[8]<<7) + data[9];
        if (flags & ID3_TAG_FLAG_FOOTERPRESENT)
            size += 10;
        for (; size < length && !data[size]; ++size);  // Consume padding 
        return size;
    }
    return 0;
}



unsigned long MpegFile::xingFrames(struct mad_bitptr ptr, unsigned bitlen)
{
#define XING_MAGIC ( ('X' << 24) | ('i' << 16) | ('n' << 8) | 'g' )

    if (bitlen >= 96 && mad_bit_read(&ptr, 32) == XING_MAGIC && (mad_bit_read(&ptr, 32) & 1 ))  // XING_FRAMES
        return mad_bit_read(&ptr, 32);

    return 0;
}



void MpegFile::madTimerMultiply(mad_timer_t* t, double d)
{
  t->seconds = (signed long)(d *= (t->seconds + t->fraction * (1. / MAD_TIMER_RESOLUTION)));
  t->fraction = (unsigned long)((d - t->seconds) * MAD_TIMER_RESOLUTION + .5);
}


