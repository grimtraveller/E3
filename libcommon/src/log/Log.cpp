

#include <stdarg.h>
#include <iostream>
#include <sstream>
#include <fstream>

#include <boost/make_shared.hpp>


#ifdef __OS_WINDOWS__
	#include <windows.h>
	#include <tchar.h>
#endif

#include <log/Log.h>
#include <log/Sinks.h>

namespace e3 { namespace log {

Logger defaultLogger;



//----------------------------------------------------------
// class Logger
//----------------------------------------------------------
//
Logger::Logger(Priority p) : 
    priority_(p)
{}


Logger::~Logger()
{
    removeSink(NULL, true);
    //for(unsigned i=0; i<sinks_.size(); i++) 
    //{ 
    //    delete sinks_[i];
    //    sinks_[i] = NULL;
    //}
    //sinks_.clear();

    attributes_.clear();
}



void Logger::initDefault()
{
    setPriority(Verbose1);
    
    attributes_.clear();
    addAttribute("Message",      boost::make_shared<InternalAttribute>(InternalAttribute(InternalAttribute::Message)));
    addAttribute("FileName",     boost::make_shared<InternalAttribute>(InternalAttribute(InternalAttribute::FileName)));
    addAttribute("FunctionName", boost::make_shared<InternalAttribute>(InternalAttribute(InternalAttribute::FunctionName)));
    addAttribute("LineNum",      boost::make_shared<InternalAttribute>(InternalAttribute(InternalAttribute::LineNum)));
    addAttribute("LineBreak",    boost::make_shared<InternalAttribute>(InternalAttribute(InternalAttribute::LineBreak)));
    addAttribute("Tab",          boost::make_shared<InternalAttribute>(InternalAttribute(InternalAttribute::Tab)));

    //addAttribute("LINEBREAK", boost::make_shared<TextAttribute>(TextAttribute("\n")));
}



bool Logger::addSink(Sink* sink)                    
{ 
    ASSERT(sink); 
    std::pair<SinkSet::iterator, bool> result = sinks_.insert(sink);

    return result.second;
}



void Logger::removeSink(Sink* sink, bool removeAll)
{
    for(SinkSet::iterator it = sinks_.begin(); it!=sinks_.end();)
    {
        Sink* current = *it;
        if(current == sink || removeAll) {
            delete current;
            it = sinks_.erase(it);
        }
        else ++it;
    }
}



void Logger::addAttribute(const std::string& name, boost::shared_ptr<Attribute> attribute)
{
    ASSERT(attribute.get());
    size_t hash = Attribute::hash(name);
    attributes_.insert( AttributeMap::value_type( hash, attribute ));
}



void Logger::output(const Record& record)
{
    for(SinkSet::iterator itSinks = sinks_.begin(); itSinks!=sinks_.end(); ++itSinks)
    {
        std::ostringstream os;

        Sink* sink = *itSinks;
        const Format& format = sink->getFormat();
        format.realize(attributes_, record, os);
        
        //for(Format::TokenIterator itFormat=format.begin(); itFormat!=format.end(); ++itFormat)
        //{
        //    const Format::Token& token = *itFormat;
        //    Format::TokenType type     = token.first;
        //    std::string data           = token.second;

        //    switch(type) 
        //    {
        //    case Format::Tag:
        //        {
        //            AttributeMap::const_iterator pos = attributes_.find(data);
        //            if(pos != attributes_.end())
        //            {
        //                Attribute* attribute = (*pos).second.get();
        //                ASSERT(attribute);
        //                attribute->realize(record, os);
        //            }
        //        }
        //        break;

        //    case Format::Text:
        //        os << data;
        //        break;

        //    default: ASSERT(false);
        //    }
        //}
        sink->output(os.str());
    }
}



void Logger::trace(const char* f, ...)
{
	char s[1000];
	va_list marker;
	va_start( marker, f );
	vsprintf( s, f, marker );

	std::cerr << s;

#ifdef __OS_WINDOWS__
	std::basic_ostringstream< TCHAR > os;
	os << s;
	::OutputDebugString(os.str().c_str());
#endif
}



//--------------------------------------------------
// class Core
//--------------------------------------------------
//
Core::Core() :
    logger_(NULL)
{}



Logger* Core::getLogger() const       
{ 
    if(logger_ == NULL) 
        return &defaultLogger;
    else
        return logger_; 
}

}} // namespace e3::log