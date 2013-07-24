
//--------------------------------------------------------
// Sinks.cpp
//--------------------------------------------------------

#include <sstream>

#ifdef __OS_WINDOWS__
    #include <windows.h>
    #include <tchar.h>
#endif

#include <log/Sinks.h>


namespace e3 { namespace log {


//----------------------------------------------------------
// class Sink
//----------------------------------------------------------
//
Sink::Sink(const std::string& formatString)
{
    setFormat(formatString);
}



void Sink::setFormat(const std::string& formatString)  
{ 
    format_.parse(formatString);
}



//----------------------------------------------------------
// class StreamSink
//----------------------------------------------------------
//
StreamSink::StreamSink(std::ostream* stream, bool isOwner) :
    stream_(stream),
    isOwner_(isOwner)
{}



StreamSink::~StreamSink()
{
    if(isOwner_ && stream_) {
        delete stream_;
    }
}



void StreamSink::output(const std::string& msg)
{
    if(stream_) {
        *stream_ << msg;
        stream_->flush(); 
    }
}


//----------------------------------------------------------
// class FileSink
//----------------------------------------------------------
//
FileSink::FileSink(const std::string& filename) : Sink()
{
    ofs_.open(filename.c_str(), std::ofstream::out | std::ofstream::app);
}



void FileSink::output(const std::string& msg)
{
    if(ofs_.is_open()) {
        ofs_ << msg;
    }
}



//----------------------------------------------------------
// class DebugSink
//----------------------------------------------------------
//
#ifdef __OS_WINDOWS__

void DebugSink::output(const std::string& msg)
{
	std::basic_ostringstream< TCHAR > os;
	os << msg.c_str();
	::OutputDebugString(os.str().c_str());
}
#endif // __OS_WINDOWS__
	
	
}} // namespace e3::log