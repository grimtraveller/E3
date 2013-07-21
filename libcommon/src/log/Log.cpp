

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
Logger::Logger(LogLevel level) : 
    level_(level)
{}


Logger::~Logger()
{
    //for(unsigned i=0; i<sinks_.size(); i++) 
    //{ 
    //    delete sinks_[i];
    //    sinks_[i] = NULL;
    //}
    sinks_.clear();
    attributes_.clear();
}



void Logger::initDefault()
{
    setLevel(Verbose1);
    
    attributes_.clear();
    addAttribute("Message",      boost::make_shared<InternalAttribute>(InternalAttribute(InternalAttribute::Message)));
    addAttribute("FileName",     boost::make_shared<InternalAttribute>(InternalAttribute(InternalAttribute::FileName)));
    addAttribute("FunctionName", boost::make_shared<InternalAttribute>(InternalAttribute(InternalAttribute::FunctionName)));
    addAttribute("LineNum",      boost::make_shared<InternalAttribute>(InternalAttribute(InternalAttribute::LineNum)));
    addAttribute("LineBreak",    boost::make_shared<InternalAttribute>(InternalAttribute(InternalAttribute::LineBreak)));
    addAttribute("Tab",          boost::make_shared<InternalAttribute>(InternalAttribute(InternalAttribute::Tab)));

    //addAttribute("LINEBREAK", boost::make_shared<TextAttribute>(TextAttribute("\n")));
}



void Logger::addSink(Sink* sink)                    
{ 
    ASSERT(sink); 
    sinks_.push_back(sink); 
}



void Logger::addAttribute(const std::string& name, boost::shared_ptr<Attribute> attribute)
{
    ASSERT(attribute.get());
    attributes_.insert( AttributeMap::value_type( name, attribute ));
}



void Logger::output(const Record& record)
{
    for(unsigned i=0; i<sinks_.size(); i++)
    {
        Sink* sink = sinks_[i];
        const Format& format = sink->getFormat();
        
        std::ostringstream os;
        for(Format::TokenIterator it=format.begin(); it!=format.end(); ++it)
        {
            const Format::Token& token = *it;
            Format::TokenType type     = token.first;
            std::string data           = token.second;

            switch(type) 
            {
            case Format::Tag:
                {
                    AttributeMap::const_iterator it = attributes_.find(data);
                    if(it != attributes_.end())
                    {
                        Attribute* attribute = (*it).second.get();
                        ASSERT(attribute);
                        attribute->realize(record, os);
                    }
                }
                break;

            case Format::Text:
                os << data;
                break;

            default: ASSERT(false);
            }
        }
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
// class LogCore
//--------------------------------------------------
//
LogCore::LogCore() :
    logger_(NULL)
{}


LogCore::LogCore( const LogCore& ) {}


Logger* LogCore::getLogger() const       
{ 
    if(logger_ == NULL) 
        return &defaultLogger;
    else
        return logger_; 
}

}} // namespace e3::log