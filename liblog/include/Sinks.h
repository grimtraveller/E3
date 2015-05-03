
//--------------------------------------------------------
// Sinks.h
//--------------------------------------------------------

#pragma once

#include <string>
#include <fstream>
#include <set>

#include <boost/utility.hpp>

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
    Sink(const std::string& formatString="%Message%%LineBreak%");       // TODO find better place for default
    virtual ~Sink() {}

    virtual void output(const std::string& msg) = 0;

    virtual void setFormat(const std::string& formatString);
    virtual const Format& getFormat() const { return format_; }

protected:
    Format format_;
};

typedef std::set<Sink*> SinkSet;


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



class FileSink : 
    public Sink, 
    private boost::noncopyable   // non-copyable, since the file resource may not be copied
{
public:
    FileSink(const std::string& filename);
    ~FileSink() {}

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