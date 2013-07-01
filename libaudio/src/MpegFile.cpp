
//--------------------------------------------------------
// AudioFormatManager.cpp
//--------------------------------------------------------

#include <cstdio>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <AudioBuffer.h>
#include <AudioFormat.h>
#include <FormatManager.h>
#include <MadDecoder.h>
#include <MpegFile.h>



MpegFile::MpegFile() : AudioFile(),
    handle_(NULL),
    decoder_(NULL)
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
    default:       EXCEPTION(std::exception, "Only read mode is suported for MPEG");
    }

    if(handle_ == NULL) {
        EXCEPTION(std::exception, "%s: %s", strerror(errno), filename_.string().c_str());
    }

    size_t bufferSize = 8192;
    decoder_ = new MadDecoder();
    decoder_->start(handle_, bufferSize);

    sampleRate_   = decoder_->getSampleRate();
    numChannels_  = decoder_->getNumChannels();
    numFrames_    = decoder_->getNumFrames();
    format_       = FormatManager::getFormat(FORMAT_MPEG);
    codec_        = FormatManager::getCodec(CODEC_MPEG);
}



void MpegFile::load(AudioBuffer* buffer)
{
    ASSERT(isReadable());

    try {
        buffer->setSampleRate(sampleRate_);
        buffer->setNumChannels(numChannels_);

        size_t numSamples = (size_t)(numFrames_ * numChannels_);
        buffer->resize(numSamples);

        if(buffer->getSize() == numSamples)
        {
            int numProcessed = decoder_->decode(numSamples, buffer);
            //ASSERT(numProcessed == numSamples);   // TODO: check why this fails
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
    if(decoder_) {
        delete decoder_;
        decoder_ = NULL;
    }
    if(handle_) {
        fclose(handle_);
        handle_ = NULL;
    }
}


//-----------------------------------------------------------
// Protected members
//-----------------------------------------------------------

bool MpegFile::isSeekable() const
{
  if(handle_ == NULL)
      return false;

  struct stat st;
  fstat(fileno(handle_), &st);

  return ((st.st_mode & S_IFMT) == S_IFREG);
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



