
//--------------------------------------------------------
// AudioFile.cpp
//--------------------------------------------------------

#include <AudioBuffer.h>
#include <AudioFile.h>
#include <InstrumentChunk.h>


AudioFile::AudioFile() :
    sampleRate_(44100),
    numChannels_(2),
    numFrames_(0),
    fileOpenMode_(OpenRead),
    instrumentChunk_(NULL)
{}



AudioFile::~AudioFile()
{}



void AudioFile::open(const Path& filename, FileOpenMode mode) 
{
    filename_     = filename;
    fileOpenMode_ = mode;
    ASSERT(filename_.empty() == false);
}


