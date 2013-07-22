
#pragma once

//#include <sstream>      
#include <set>

#include <boost/utility.hpp>  // boost::noncopyable

#include <CommonMacros.h>
#include <log/Attributes.h>
#include <log/Record.h>


namespace e3 { namespace log {

enum Priority
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
    Logger(Priority p=Verbose1);
    ~Logger();

    void initDefault();
    void output(const Record& record);

    void setPriority(Priority p)               { priority_ = p; }
    Priority getPriority() const               { return priority_; }

    bool addSink(Sink* sink);
    void removeSink(Sink* sink, bool removeAll=false);

    void addAttribute(const std::string& name, boost::shared_ptr<Attribute> attribute);
    // TODO: removeAttribute
    // TODO: use hash_map or integer-id
    // TODO: force uniqueness of attributes in the map
    // TODO: any kind of feedback when a format contains an unknown attribute

public:
    static void trace(const char* f, ...);      // TODO: remove from class

protected:
    Priority priority_;

    typedef std::set<Sink*> SinkSet;
    SinkSet sinks_;

    //typedef std::map<const std::string, boost::shared_ptr<Attribute> > AttributeMap;
    AttributeMap attributes_;
};




//-----------------------------------------------------------------------------------------
// class Core
// Singleton class to manage Loggers
//
// TODO: manage multiple loggers
//-----------------------------------------------------------------------------------------
class Core : private boost::noncopyable
{
    DECLARE_SINGLETON( Core )

public:
    Logger* getLogger() const;
    void setLogger(Logger* logger)  { logger_ = logger; }

private:
    Logger* logger_;
};

extern Logger defaultLogger;

}} // namespace e3::log


// Sends a trace message to all outputs supported by the Trace class.
// Use printf-style to format the message.
//
#define TRACE( ... ) e3::log::Logger::trace( ##__VA_ARGS__ )


#define LOG(priority)                                                           \
    if(priority < e3::log::Core::instance().getLogger()->getPriority()) ;    \
    else e3::log::Record(__FILE_ONLY__, __FUNCTION__, __LINE__).getStream()    



