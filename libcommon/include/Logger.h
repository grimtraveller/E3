
#pragma once

#include <iostream>     // ostream
//#include <iosfwd>
#include <sstream>      // ostringstream
#include <vector>
#include <map>

#include <boost/shared_ptr.hpp>

#include <LibCommon.h>



enum LogLevel
{
    Log_Verbose1,
    Log_Verbose2,
    Log_Warning,
    Log_Error,
};

class Logger;


//-----------------------------------------------------------------------------------------
// Singleton class to manage Loggers
//-----------------------------------------------------------------------------------------
class LogCore
{
    DECLARE_SINGLETON( LogCore )

public:
    Logger* getLogger() const;
    void setLogger(Logger* logger)  { logger_ = logger; }

    static const std::string getDefaultFormat()    { return "%MESSAGE%%LINEBREAK%"; }

private:
    Logger* logger_;
};


//-----------------------------------------------------------------------------------------
// class FormatParser
// Parses a format string and stores the tokens in a vector
//-----------------------------------------------------------------------------------------
//
class FormatParser
{
public:
    typedef std::vector<std::string> TokenVector;

    const TokenVector& parse(const std::string& format);

protected:
    TokenVector tokens_;
};



//-----------------------------------------------------------------------------------------
// class LogSinks
// Abstract base class for log sinks
//-----------------------------------------------------------------------------------------
//
class LogSink
{
public:
    LogSink() {}
    virtual ~LogSink() {}
    virtual void output(const std::string& msg) = 0;

    virtual const std::string getFormat() const  // TODO: return reference not value
    { 
        return format_.empty() ? LogCore::getDefaultFormat() : format_; 
    }
    virtual void setFormat(const std::string& f);

protected:
    std::string format_;

    void parseFormat(const std::string& format);
};



class StreamLogSink : public LogSink
{
public:
    StreamLogSink(std::ostream* stream, bool isOwner=false);
    ~StreamLogSink();
    
    void output(const std::string& msg);

protected:
    std::ostream* stream_;
    bool isOwner_;
};



class FileLogSink : public LogSink
{
public:
    FileLogSink(const std::string& filename);
    ~FileLogSink();

    void output(const std::string& msg);

protected:
    std::ofstream ofs_;
};



#ifdef __OS_WINDOWS__

class DebugLogSink : public LogSink
{
public:
    void output(const std::string& msg);
};
#endif // __OS_WINDOWS__


//-----------------------------------------------------------------------------------------
// class LogAttribute
// Interface for log attributes. These are configurable token to be inserted to LogRecords
//-----------------------------------------------------------------------------------------
//
class LogAttribute
{
public:
    class Impl
    {
    public:
        virtual const std::string& getString() const = 0;
    };

    LogAttribute() : impl_(NULL) {}
    LogAttribute(Impl* impl) : impl_(impl) {}
    ~LogAttribute() {}

    virtual const std::string& getString() const
    { 
        ASSERT(impl_);
        return impl_->getString(); 
    }

    //friend std::ostream& operator<< (std::ostream& out, LogAttribute& attr);
protected:
    Impl* impl_;  // TODO: make this intrusive_ptr
};




//-----------------------------------------------------------------------------------------
// class TextAttribute
//-----------------------------------------------------------------------------------------
//
class TextAttribute : public LogAttribute
{
public:
    TextAttribute(const std::string& text="") : 
        LogAttribute(new Impl(text)) 
    {}

protected:
    class Impl : public LogAttribute::Impl
    {
    public:
        Impl(const std::string& text) : text_(text) {}
        const std::string& getString() const        { return text_; }

    protected:
        std::string text_;
    };
};


////-----------------------------------------------------------------------------------------
//// class LinebreakAttribute
////-----------------------------------------------------------------------------------------
//class LinebreakAttribute : public LogAttribute
//{
//public:
//    LinebreakAttribute() : LogAttribute(new Impl()) 
//    {}
//
//protected:
//    class Impl : public LogAttribute::Impl
//    {
//    public:
//        Impl() : linebreak_("\n")            {}
//        const std::string& getString() const { return linebreak_; }
//
//    protected:
//        std::string linebreak_;
//    };
//};

// TODO: 
// file attribute
// line attribute
// timeline attribute
// timestamp attribute
// counter attribute



//-----------------------------------------------------------------------------------------
// class Logger
// Dispatches log messages to sinks
//-----------------------------------------------------------------------------------------
//
class Logger
{
public:
    Logger(LogLevel level=Log_Verbose1);
    ~Logger();

    void initDefault();
    void output(const std::string& os);

    void setLevel(LogLevel level)   { level_ = level; }
    LogLevel getLevel() const       { return level_; }

    void addSink(LogSink* sink)     { sinks_.push_back(sink); }
    // TODO: removeSink

    void addAttribute(const std::string& name, const LogAttribute& attribute);

public:
    static void trace(const char* f, ...);      // TODO: remove from class

protected:
    LogLevel level_;

    typedef std::vector<LogSink*> SinkVector;
    SinkVector sinks_;

    typedef std::map<const std::string, LogAttribute> AttributeMap;
    AttributeMap attributes_;
};


extern Logger defaultLogger;

//-----------------------------------------------------------------------------------------
// class LogRecord
// Represents a single log entity
//-----------------------------------------------------------------------------------------
//
class LogRecord
{
public:
    LogRecord(const char* file, const char* function, int line); // TODO: char* won't work when LogRecords are cached
    ~LogRecord();

    std::ostringstream& getStream() { return stream_; }
    //void flush();

protected:
    std::ostringstream stream_;

    const char* file_;
    const char* function_;
    int line_;
};






// Sends a trace message to all outputs supported by the Trace class.
// Use printf-style to format the message.
//
#define TRACE( ... ) Logger::trace( ##__VA_ARGS__ )


#define LOG(level)                          \
    if(level < LogCore::instance().getLogger()->getLevel()) ;        \
    else LogRecord(__FILE_ONLY__, __FUNCTION__, __LINE__).getStream()    



