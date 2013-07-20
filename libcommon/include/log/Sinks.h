
//--------------------------------------------------------
// Sinks.h
//--------------------------------------------------------

#pragma once

#include <fstream>

#include <log/Format.h>

namespace e3 { namespace log {

//-----------------------------------------------------------------------------------------
// class Sink
// Abstract base class for log sinks
//-----------------------------------------------------------------------------------------
//
class Sink
{
public:
    Sink() {}
    virtual ~Sink() {}
    virtual void output(const std::string& msg) = 0;

    //virtual const std::string getFormat() const  // TODO: return reference not value
    //{ 
    //    return format_.empty() ? LogCore::getDefaultFormat() : format_; 
    //}
    virtual void setFormat(const std::string& formatString);
    virtual const Format& getFormat() const { return format_; }

protected:
    //std::string format_;
    //void parseFormat(const std::string& format);
    Format format_;
};



class StreamSink : public Sink
{
public:
    StreamSink(std::ostream* stream, bool isOwner=false);
    ~StreamSink();
    
    void output(const std::string& msg);

protected:
    std::ostream* stream_;
    bool isOwner_;
};



class FileSink : public Sink
{
public:
    FileSink(const std::string& filename);
    ~FileSink();

    void output(const std::string& msg);

protected:
    std::ofstream ofs_;
};



#ifdef __OS_WINDOWS__

class DebugSink : public Sink
{
public:
    void output(const std::string& msg);
};
#endif // __OS_WINDOWS__


	
	
}} // namespace e3::log