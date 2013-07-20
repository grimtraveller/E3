

#include <stdarg.h>
#include <iostream>
#include <sstream>
#include <fstream>


#ifdef __OS_WINDOWS__
	#include <windows.h>
	#include <tchar.h>
#endif

#include <log/Log.h>
#include <log/Sinks.h>

namespace e3 { namespace log {

Logger defaultLogger;



//--------------------------------------------------
// class LogRecord
//--------------------------------------------------
//
LogRecord::LogRecord(const char* file, const char* function, int line) :
    file_(file),
    function_(function),
    line_(line)
{
    //std::cout << "FILENAME="<<filename<<std::endl;
    //std::cout << "FUNCTIONNAME="<<functionname<<std::endl; 
    //std::cout << "LINENUMBER="<<linenumber<<std::endl;
}


LogRecord::~LogRecord()
{
    stream_.flush();
    Logger* logger = LogCore::instance().getLogger();
    logger->output(*this);
}



//----------------------------------------------------------
// class Logger
//----------------------------------------------------------
//
Logger::Logger(LogLevel level) : 
    level_(level)
{}


Logger::~Logger()
{
    for(unsigned i=0; i<sinks_.size(); i++) 
    { 
        delete sinks_[i];
        sinks_[i] = NULL;
    }
    sinks_.clear();
}



void Logger::initDefault()
{
    setLevel(Verbose1);
    
    attributes_.clear();
    addAttribute("MESSAGE", TextAttribute());
    addAttribute("LINEBREAK", TextAttribute("\n"));
    //addAttribute("FILE", InternalAttribute<std::string>(internalAttribute_));
}



void Logger::addAttribute(const std::string& name, const Attribute& attribute)
{
    const std::string key = "%" + name + "%"; 
    attributes_.insert( AttributeMap::value_type( key, attribute ));
}



void Logger::output(const LogRecord& record)
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
                if(data == "MESSAGE")
                    os << record.getMessage();
                else if(data == "LINEBREAK")
                    os << std::endl;
                break;

            case Format::Text:
                os << data;
                break;

            default: ASSERT(false);
            }
        }

        //AttributeMap::const_iterator it = attributes_.begin();
        //for(; it!=attributes_.end(); ++it)
        //{
        //    const std::string& name  = it->first;
        //    const LogAttribute& attr = it->second;

        //    const std::string& value = (name != "%MESSAGE%") ? attr.getString() : msg;
        //    //format = boost::algorithm::replace_all_copy(format, name, value);
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