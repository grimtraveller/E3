
#pragma once

#include <sstream>      
#include <vector>
#include <map>

#include <boost/shared_ptr.hpp>

#include <CommonMacros.h>
#include <log/Attributes.h>
#include <log/Record.h>


namespace e3 { namespace log {

enum LogLevel
{
    Verbose1,
    Verbose2,
    Warning,
    Error,
};

// Forwards
class Sink;


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
    void output(const Record& record);

    void setLevel(LogLevel level)               { level_ = level; }
    LogLevel getLevel() const                   { return level_; }

    void addSink(Sink* sink);
    // TODO: removeSink

    void addAttribute(const std::string& name, boost::shared_ptr<Attribute> attribute);
    // TODO: removeAttribute

public:
    static void trace(const char* f, ...);      // TODO: remove from class

protected:
    LogLevel level_;

    typedef std::vector<Sink*> SinkVector;
    SinkVector sinks_;

    typedef std::map<const std::string, boost::shared_ptr<Attribute> > AttributeMap;
    AttributeMap attributes_;
};




//-----------------------------------------------------------------------------------------
// Singleton class to manage Loggers
//-----------------------------------------------------------------------------------------
class LogCore
{
    DECLARE_SINGLETON( LogCore )

public:
    Logger* getLogger() const;
    void setLogger(Logger* logger)  { logger_ = logger; }

    static const std::string getDefaultFormat()    { return "%Message%%LineBreak%"; }

private:
    Logger* logger_;
};






extern Logger defaultLogger;

}} // namespace e3::log


// Sends a trace message to all outputs supported by the Trace class.
// Use printf-style to format the message.
//
#define TRACE( ... ) e3::log::Logger::trace( ##__VA_ARGS__ )


#define LOG(level)                          \
    if(level < e3::log::LogCore::instance().getLogger()->getLevel()) ;        \
    else e3::log::Record(__FILE_ONLY__, __FUNCTION__, __LINE__).getStream()    



