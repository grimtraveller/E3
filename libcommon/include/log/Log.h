
//-------------------------------------------------------------------
// Log.h
//
// Core classes for log
//-------------------------------------------------------------------

#pragma once

#include <sstream>
#include <set>
#include <vector>

#include <log/RecordBuffer.h>
#include <log/Attributes.h>
#include <log/Sinks.h>
#include <log/Record.h>


namespace e3 { namespace log {


//-----------------------------------------------------------------------------------------
// class Logger
// Dispatches log messages to sinks
//-----------------------------------------------------------------------------------------
//
class Logger
{
public:
    Logger(bool useDefault=true, size_t bufferSize=10);

    virtual ~Logger();

    void initDefault();
    void pushRecord(const RecordBase& record);

    bool addSink(Sink* sink);
    void removeSink(Sink* sink);
    void clearSinks();
    const SinkSet& getSinks() const             { return sinks_; }

    bool addAttribute(const std::string& name, boost::shared_ptr<Attribute> attribute);
    void removeAttribute(const std::string& name);
    const AttributeMap& getAttributes() const   { return attributes_; }

    void lock()                                 { lock_ = true; }
    void unlock()                               { lock_ = false; }
    bool isLocked() const                       { return lock_; }
    void flush();

    void resizeBuffer(size_t size)              { buffer_.resize(size); }
    void clearBuffer()                          { buffer_.clear(); }
    void setOverflowPolicy(OverflowPolicy p)    { buffer_.setOverflowPolicy(p); }

protected:
    SinkSet sinks_;
    AttributeMap attributes_;

    bool lock_;

    typedef RecordBuffer<PersistentRecord> RecordBufferT;
    RecordBufferT buffer_;

    void output(const RecordBase& record);

    friend class RecordBase;
    virtual void priorityToString( std::ostringstream& stream, int priority) {}
};



enum Priority
{
    NOTSET,
    DEBUG,  // Information useful during development for debugging.
    INFO,   // Normal operational messages. No action required.
    NOTICE, // Events that are unusual, but not error conditions.
    WARN,   // Not an error, but indicates error will occurr if nothing done.
    ERR,    // Non-urgent failures; relay to developers
    CRIT,   // Failure in non-primary system (e.g. backup site down)
    ALERT,  // System is unusable (e.g. single part down)
    EMERG   // System is unusable (e.g. multiple parts down)
};


std::ostringstream& operator<< (std::ostringstream& stream, Priority);


template< typename PriorityT=Priority >
class PriorityLogger : public Logger
{
public:
    typedef PriorityT Priority;

    PriorityLogger(Priority p, bool useDefault=true) : 
        Logger(useDefault),
        priority_(p)
    {}

    void setPriority(Priority p)  { priority_ = p; }
    Priority getPriority() const  { return priority_; }

protected:
    Priority priority_;

    void priorityToString( std::ostringstream& os, int priority)
    {
        Priority p = static_cast<Priority>(priority);
        os << p;
    }
};


extern Logger defaultLogger;

}} // namespace e3::log



#define E3_LOG()                                                                       \
    e3::log::StreamRecord(&defaultLogger, __FILE_ONLY__, __FUNCTION__, __LINE__).getStream()    


#define E3_LOG_PRIO(priority, logger)                                                           \
    if(priority < logger.getPriority()) ;                                                       \
    else e3::log::StreamRecord(&logger, priority, __FILE_ONLY__, __FUNCTION__, __LINE__).getStream() 


#define E3_LOG_PRIO_BUF(priority, logger, format, ...)                                                           \
    if(priority < logger.getPriority()) ;                                                       \
    else e3::log::BufferRecord<255>(&logger, priority, __FILE_ONLY__, __FUNCTION__, __LINE__, format, __VA_ARGS__)
