

#include <stdarg.h>
#include <iostream>
#include <sstream>
#include <fstream>

#include <boost/algorithm/string/replace.hpp>


#ifdef __OS_WINDOWS__
	#include <windows.h>
	#include <tchar.h>
#endif

#include <Logger.h>

Logger defaultLogger;


//----------------------------------------------------------
// class LogRecord
//----------------------------------------------------------

LogRecord::~LogRecord()
{
    //flush();
    stream_.flush();
    Logger* l = LogCore::instance().getLogger();
    l->output(stream_.str());
}


//void LogRecord::flush()
//{
//    stream_ << std::endl;
//}


//----------------------------------------------------------
// class LogSink
//----------------------------------------------------------
//


void LogSink::setFormat(const std::string& f)  
{ 
    format_ = f; 

    parseFormat(format_);

    int x = 0;
}



//#include <boost/config/warning_disable.hpp>     // experimental
#include <boost/spirit/include/qi.hpp>

//#include <boost/config/warning_disable.hpp>
////#include <boost/spirit/include/qi.hpp>
//#include <boost/spirit/include/phoenix_core.hpp>
//#include <boost/spirit/include/phoenix_operator.hpp>
//#include <boost/spirit/include/phoenix_object.hpp>
//#include <boost/fusion/include/adapt_struct.hpp>
//#include <boost/fusion/include/io.hpp>


namespace client
{
    namespace qi = boost::spirit::qi;
    //namespace ascii = boost::spirit::ascii;

    struct TagFunctor
    {
        void operator()(const std::string& str, qi::unused_type, qi::unused_type) const 
        {
            std::cout << "Tag: <" << str << ">   length="<<str.length() << std::endl;
        }
    };

    struct TextFunctor
    {
        void operator()(const std::string& str, qi::unused_type, qi::unused_type) const 
        {
            std::cout << "Text: <" << str << ">   length="<<str.length() << std::endl;
        }
    };


    void parseTags(const std::string& s)
    {
        TagFunctor functorTag;
        TextFunctor functorText;

        typedef std::string::const_iterator IteratorT;
        typedef qi::rule<IteratorT, std::string()> StringRuleT;
        typedef qi::rule<IteratorT, char()> CharRuleT;
        
        CharRuleT escChar  = '\\' >> qi::char_("\\%"); 
        StringRuleT tag    = '%' >> +(qi::char_ - '%') >> '%';
        StringRuleT text   = +(escChar | ~qi::char_('%'));
        StringRuleT format = *(text[functorText] | tag[functorTag]); 

        bool result = qi::parse(s.begin(), s.end(), format);
        
        std::cout << "parseTags " << (result?"successful":"failed") << std::endl;
        std::cout << std::endl;
    }
}


void LogSink::parseFormat(const std::string& format)
{
    //std::string tags = "%TIMESTAMP%  %FILE% %MESSAGE% bla bla %LINEBREAK%";
    
    std::string tags = "12\\%3 %MESSAGE%###%QUATSCH% hulla bulla *** %LINEBREAK%%LINEBREAK%";

    client::parseTags(tags);
}




//----------------------------------------------------------
// class StreamLogSink
//----------------------------------------------------------

StreamLogSink::StreamLogSink(std::ostream* stream, bool isOwner) :
    stream_(stream),
    isOwner_(isOwner)
{}



StreamLogSink::~StreamLogSink()
{
    if(isOwner_ && stream_) {
        delete stream_;
    }
}



void StreamLogSink::output(const std::string& msg)
{
    if(stream_) {
        *stream_ << msg;
        stream_->flush();
    }
}


//----------------------------------------------------------
// class FileLogSink
//----------------------------------------------------------

FileLogSink::FileLogSink(const std::string& filename)
{
    ofs_.open(filename, std::ofstream::out | std::ofstream::app);
}


FileLogSink::~FileLogSink()
{
    ofs_.close();
}


void FileLogSink::output(const std::string& msg)
{
    if(ofs_.is_open()) {
        ofs_ << msg;
    }
}



//----------------------------------------------------------
// class DebugLogSink
//----------------------------------------------------------
#ifdef __OS_WINDOWS__

void DebugLogSink::output(const std::string& msg)
{
	std::basic_ostringstream< TCHAR > os;
	os << msg.c_str();
	::OutputDebugString(os.str().c_str());
}
#endif // __OS_WINDOWS__




//----------------------------------------------------------
// class LogAttribute
//----------------------------------------------------------
//
//std::ostream& operator<< (std::ostream& out, LogAttribute& attr) // TODO: why?
//{
//    out << attr.getString();
//    return out;
//}

//----------------------------------------------------------
// class LogAttribute
//----------------------------------------------------------
//




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
    setLevel(Log_Verbose1);
    
    attributes_.clear();
    addAttribute("MESSAGE", TextAttribute());
    addAttribute("LINEBREAK", TextAttribute("\n"));
    //addAttribute("FILE", InternalAttribute<std::string>(internalAttribute_));
}



void Logger::addAttribute(const std::string& name, const LogAttribute& attribute)
{
    const std::string key = "%" + name + "%"; 
    attributes_.insert( AttributeMap::value_type( key, attribute ));
}



void Logger::output(const std::string& msg)
{
    for(unsigned i=0; i<sinks_.size(); i++)
    {
        LogSink* sink = sinks_[i];
        std::string format = sink->getFormat();

        AttributeMap::const_iterator it = attributes_.begin();
        for(; it!=attributes_.end(); ++it)
        {
            const std::string& name  = it->first;
            const LogAttribute& attr = it->second;

            const std::string& value = (name != "%MESSAGE%") ? attr.getString() : msg;
            format = boost::algorithm::replace_all_copy(format, name, value);
        }
        sink->output(format);
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
// class LogRecord
//--------------------------------------------------

LogRecord::LogRecord(const char* file, const char* function, int line) :
    file_(file),
    function_(function),
    line_(line)
{
    //std::cout << "FILENAME="<<filename<<std::endl;
    //std::cout << "FUNCTIONNAME="<<functionname<<std::endl; 
    //std::cout << "LINENUMBER="<<linenumber<<std::endl;
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