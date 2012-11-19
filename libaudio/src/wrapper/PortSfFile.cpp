
//#include <stdlib.h>
//#include <stdio.h>
#include <exception>

#include <wrapper/PortSfFile.h>



PortSfFile::FormatMapper PortSfFile::formatMapper_s;
PortSfFile::EncodingMapper PortSfFile::encodingMapper_s;
PortSfFile::Initializer PortSfFile::initializer_s;


PortSfFile::Initializer::Initializer()
{
	psf_init();

    formatMapper_s.add( FMT_WAV,   PSF_STDWAVE );
    formatMapper_s.add( FMT_AIFF,  PSF_AIFF );
    formatMapper_s.add( FMT_AIFC,  PSF_AIFC );
    formatMapper_s.add( FMT_WAVEX, PSF_WAVE_EX );

	encodingMapper_s.add( ENC_UNKNOWN, PSF_SAMP_UNKNOWN ); 
    encodingMapper_s.add( ENC_PCM_S8,  PSF_SAMP_8 ); 
	encodingMapper_s.add( ENC_PCM_16,  PSF_SAMP_16 ); 
	encodingMapper_s.add( ENC_PCM_24,  PSF_SAMP_24 );
	encodingMapper_s.add( ENC_PCM_32,  PSF_SAMP_32 );
    encodingMapper_s.add( ENC_FLOAT,   PSF_SAMP_IEEE_FLOAT );
}


PortSfFile::Initializer::~Initializer()
{
    psf_finish();
}



PortSfFile::PortSfFile() :
	AudioFile(),
	handle_(-1)
{}


PortSfFile::~PortSfFile()
{
	close();
}


void PortSfFile::open(const Path& filename, FileOpenMode mode)
{
    AudioFile::open(filename, mode);

    if(mode != OpenRead) {
        EXCEPTION(std::exception, "Only read-only access to files supported.");
    }

    PSF_PROPS properties;
    memset(&properties, 0, sizeof(properties));

	handle_ = psf_sndOpen(filename_.string().c_str(), &properties, 0);

	if(handle_ >= 0) 
    {
		format_      = formatMapper_s.getLeft(properties.format);
        encoding_    = encodingMapper_s.getLeft(properties.samptype);
        sampleRate_  = properties.srate;
		numChannels_ = properties.chans;
		numFrames_   = psf_sndSize(handle_);
	}
}


void PortSfFile::close()
{
	if(handle_ > -1) 
    {
		psf_sndClose(handle_);
        handle_ = -1;
	}
}


int64 PortSfFile::seek(int64 frame)
{
	int result = psf_sndSeek(handle_, (int)frame, PSF_SEEK_SET);

	if(result == PSF_E_NOERROR) {
        int pos = psf_sndTell(handle_);
        ASSERT(pos == frame);
        return pos;
    }
    return -1;
}



int64 PortSfFile::readFloat(float* buffer, int64 num)
{
    seek(0);

    // psf_sndReadFloatFrames reads items, not frames. So we have to convert.
    ldiv_t divResult = ldiv((long)num, (long)numChannels_);
    ASSERT(divResult.rem == 0);

    int numRead = psf_sndReadFloatFrames(handle_, buffer, divResult.quot);

    return numRead * numChannels_;
}


int64 PortSfFile::readDouble(double* buffer, int64 num)
{
    seek(0);

    // psf_sndReadFloatFrames reads items, not frames. So we have to convert.
    ldiv_t divResult = ldiv((long)num, (long)numChannels_);
    ASSERT(divResult.rem == 0);

    int numRead = psf_sndReadDoubleFrames(handle_, buffer, divResult.quot);

    return numRead * numChannels_;
}

std::string PortSfFile::getErrorString() const
{
    return "error string not implemented";
}


