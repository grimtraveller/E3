

#include <iostream>
#include <sstream>
#include <fstream>

#include <boost/make_shared.hpp>

#include <CommonMacros.h>
#include <log/Log.h>
#include <log/Sinks.h>

namespace e3 { namespace log {

Logger defaultLogger;



//----------------------------------------------------------
// class Logger
//----------------------------------------------------------
//
Logger::Logger(bool useDefault, size_t bufferSize) :
    lock_(false)
{
    buffer_.resize(bufferSize);
    //records_.reserve(bufferSize); 
    
    if(useDefault)
        initDefault();
}


Logger::~Logger()
{
    clearSinks(); 
    attributes_.clear();
}



void Logger::initDefault()
{
    attributes_.clear();
    addAttribute("Message",      boost::make_shared<InternalAttribute>(InternalAttribute(InternalAttribute::Message)));
    addAttribute("Priority",     boost::make_shared<InternalAttribute>(InternalAttribute(InternalAttribute::Priority)));
    addAttribute("FileName",     boost::make_shared<InternalAttribute>(InternalAttribute(InternalAttribute::FileName)));
    addAttribute("FunctionName", boost::make_shared<InternalAttribute>(InternalAttribute(InternalAttribute::FunctionName)));
    addAttribute("LineNum",      boost::make_shared<InternalAttribute>(InternalAttribute(InternalAttribute::LineNum)));
    addAttribute("LineBreak",    boost::make_shared<InternalAttribute>(InternalAttribute(InternalAttribute::LineBreak)));
    addAttribute("Tab",          boost::make_shared<InternalAttribute>(InternalAttribute(InternalAttribute::Tab)));
}



bool Logger::addSink(Sink* sink)                    
{ 
    ASSERT(sink); 
    std::pair<SinkSet::iterator, bool> result = sinks_.insert(sink);

    return result.second;
}



void Logger::removeSink(Sink* sink)
{
    for(SinkSet::iterator it = sinks_.begin(); it!=sinks_.end();)
    {
        Sink* current = *it;
        if(current == sink) {
            delete current;
            it = sinks_.erase(it);
        }
        else ++it;
    }
}


void Logger::clearSinks()
{
    for(SinkSet::iterator it = sinks_.begin(); it!=sinks_.end(); ++it)
    {
        Sink* current = *it;
        delete current;
    }
    sinks_.clear();
}


bool Logger::addAttribute(const std::string& name, boost::shared_ptr<Attribute> attribute)
{
    ASSERT(attribute.get());

    size_t hash = Attribute::hash(name);
    std::pair<AttributeMap::iterator, bool> result = 
        attributes_.insert( AttributeMap::value_type( hash, attribute ));

    return result.second;
}



void Logger::removeAttribute(const std::string& name)
{
    size_t hash = Attribute::hash(name);
    attributes_.erase(hash);
}



void Logger::pushRecord(const RecordBase& record)
{
    if(lock_)
    {
        //records_.emplace_back(record);
        PersistentRecord& p = buffer_.getElementAtWritePosition();
        p = record;
        buffer_.push(p);
    }
    else 
        output(record);
}



void Logger::output(const RecordBase& record)
{
    for(SinkSet::iterator itSinks = sinks_.begin(); itSinks!=sinks_.end(); ++itSinks)
    {
        std::ostringstream os;
        Sink* sink = *itSinks;
        const Format& format = sink->getFormat();

        format.realize(attributes_, record, os);
        sink->output(os.str());
    }
}



void Logger::flush()
{
    while(buffer_.empty() == false)
    {
        const PersistentRecord& r = buffer_.pop();
        output(r);
    }
}



std::ostringstream& operator<< (std::ostringstream& stream, Priority priority)
{
    static const char* strings[] =
    {
        "NOTSET",
        "DEBUG", 
        "INFO",  
        "NOTICE",
        "WARN",  
        "ERROR", 
        "CRIT",  
        "ALERT", 
        "EMERG"  
    };

    if (static_cast< std::size_t >(priority) < sizeof(strings) / sizeof(*strings))
        stream << strings[priority];
    else
        stream << static_cast< int >(priority);

    return stream;
}

}} // namespace e3::log