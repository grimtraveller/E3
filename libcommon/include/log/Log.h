
#pragma once

//#include <iostream>     // ostream
//#include <iosfwd>
#include <sstream>      // ostringstream
#include <vector>
#include <map>

#include <boost/shared_ptr.hpp>

#include <CommonMacros.h>
#include <log/Attributes.h>
#include <log/Sinks.h>


namespace e3 { namespace log {

enum LogLevel
{
    Verbose1,
    Verbose2,
    Warning,
    Error,
};

// Forwards
class Logger;
class Sink;


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
    std::string getMessage() const  { return stream_.str(); }       // TODO: can this method return a reference?

protected:
    std::ostringstream stream_;

    const char* file_;
    const char* function_;
    int line_;
};


//-----------------------------------------------------------------------------------------
// class Logger
// Dispatches log messages to sinks
//-----------------------------------------------------------------------------------------
//
class Logger
{
public:
    Logger(LogLevel level=Verbose1);
    ~Logger();

    void initDefault();
    void output(const LogRecord& record);

    void setLevel(LogLevel level)   { level_ = level; }
    LogLevel getLevel() const       { return level_; }

    void addSink(Sink* sink)        { sinks_.push_back(sink); }
    // TODO: removeSink

    void addAttribute(const std::string& name, const Attribute& attribute);

public:
    static void trace(const char* f, ...);      // TODO: remove from class

protected:
    LogLevel level_;

    typedef std::vector<Sink*> SinkVector;
    SinkVector sinks_;

    typedef std::map<const std::string, Attribute> AttributeMap;
    AttributeMap attributes_;
};


extern Logger defaultLogger;

}} // namespace e3::log


// Sends a trace message to all outputs supported by the Trace class.
// Use printf-style to format the message.
//
#define TRACE( ... ) e3::log::Logger::trace( ##__VA_ARGS__ )


#define LOG(level)                          \
    if(level < e3::log::LogCore::instance().getLogger()->getLevel()) ;        \
    else e3::log::LogRecord(__FILE_ONLY__, __FUNCTION__, __LINE__).getStream()    



