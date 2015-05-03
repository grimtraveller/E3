
//--------------------------------------------------------
// AudioFormatManager.cpp
//--------------------------------------------------------

#include <cstdio>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <e3_Exception.h>

#include <AudioBuffer.h>
#include <AudioFormat.h>
#include <FormatManager.h>
#include <MadDecoder.h>
#include <MpegFile.h>


namespace e3 {

    MpegFile::MpegFile() : AudioFile(),
        handle_(NULL),
        decoder_(NULL)
    {}


    MpegFile::~MpegFile()
    {
        close();
    }



    void MpegFile::open(const Path& filename, FileOpenMode mode)
    {
        AudioFile::open(filename, mode);

        switch (fileOpenMode_)
        {
        case OpenRead: handle_ = fopen(filename_.string().c_str(), "rb"); break;
        default:       THROW(std::exception, "Only read mode is suported for MPEG");
        }

        if (handle_ == NULL)
            THROW(std::exception, "%s: %s", strerror(errno), filename_.string().c_str());

        decoder_ = new MadDecoder();
        numFrames_ = decoder_->start(handle_);   // estimated value (when mpeg is CBR) 

        sampleRate_ = decoder_->getSampleRate();
        numChannels_ = decoder_->getNumChannels();
        format_ = FormatManager::getFormat(FORMAT_MPEG);
        codec_ = FormatManager::getCodec(decoder_->getCodecId());
    }



    void MpegFile::load(AudioBuffer* buffer)
    {
        ASSERT(isReadable());

        try {
            buffer->setSampleRate(sampleRate_);
            buffer->setNumChannels(numChannels_);

            size_t numSamples = (size_t)(numFrames_ * numChannels_);
            buffer->resize(numSamples);

            if (buffer->size() == numSamples)
            {
                int numProcessed = decoder_->decode(numSamples, buffer);
                buffer->resize(numProcessed, false);
                numFrames_ = buffer->getNumFrames();    // now we know the real size
            }
            else THROW(std::exception, "Out of memory");
        }
        catch (const std::exception& e)
        {
            buffer->resize(0);
            throw e;
        }
    }



    void MpegFile::close()
    {
        if (decoder_) {
            delete decoder_;
            decoder_ = NULL;
        }
        if (handle_) {
            fclose(handle_);
            handle_ = NULL;
        }
    }



    std::string MpegFile::getVersionString() const
    {
        return MadDecoder::getVersionString();
    }



    //-----------------------------------------------------------
    // Static members
    //-----------------------------------------------------------

    bool MpegFile::isFormatSupported(const FormatInfo& format, const CodecInfo& codec, int sampleRate, int numChannels)
    {
        return format.id_ == FORMAT_MPEG && (codec.id_ == CODEC_MP1 || codec.id_ == CODEC_MP2 || codec.id_ == CODEC_MP3);
    }



    void MpegFile::initFormatInfos(FormatInfoVector& infos)
    {
        infos.push_back(FormatInfo(FORMAT_MPEG, -1, "MPEG", "mp3", "MPEG Layer I/II/III lossy audio compression"));
    }



    void MpegFile::initCodecInfos(CodecInfoVector& infos)
    {
        infos.push_back(CodecInfo(CODEC_MP1, -1, "MP1", "MPEG Layer I"));
        infos.push_back(CodecInfo(CODEC_MP2, -1, "MP2", "MPEG Layer II"));
        infos.push_back(CodecInfo(CODEC_MP3, -1, "MP3", "MPEG Layer III"));
    }

} // namespace e3

