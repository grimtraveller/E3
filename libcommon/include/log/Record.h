
//------------------------------------------------------------
// Record.h
//
// Stores a single log record and environmental data
// gathered from the macros __FILE__, __FUNCTION__ and __LINE__
//------------------------------------------------------------

#pragma once

#include <stdarg.h>
#include <string>
#include <sstream>

#include <boost/utility.hpp>


namespace e3 { namespace log {

class Logger;

class RecordBase
{
public:
    RecordBase();
    RecordBase(const RecordBase& rhs);
    RecordBase(Logger* logger, int priority); 
    RecordBase(Logger* logger, int priority, const char* file, const char* function, int line); 
    virtual ~RecordBase() {}                          
                                                 
    virtual void getMessage(std::string& result) const {}
    virtual int getPriority() const              { return priority_; }
    virtual Logger* getLogger() const            { return logger_; }

    virtual const char* getFile() const          { return file_; }
    virtual const char* getFunction() const      { return function_; }
    int getLine() const                          { return line_; }

protected:
    virtual void push();

    Logger* logger_;
    int priority_;

    const char* file_;
    const char* function_;
    int line_;
};



class StreamRecord : public RecordBase, public boost::noncopyable
{
public:
    StreamRecord(Logger* logger, int priority); 
    StreamRecord(Logger* logger, int priority, const char* file, const char* function, int line); 
    ~StreamRecord() { push(); }

    std::ostringstream& getStream()              { return stream_; }
    void getMessage(std::string& result) const   { result = stream_.str(); }
    
protected:
    std::ostringstream stream_;
};



//-----------------------------------------------------------------------------
// class BufferRecord
//-----------------------------------------------------------------------------
//
template<unsigned int N>
class BufferRecord : public RecordBase, public boost::noncopyable
{
public:
    BufferRecord(Logger* logger, int priority, const char* file, const char* function, int line, const char* fmt, ...) :
        RecordBase(logger, priority, file, function, line)
    {
	    va_list args;
	    va_start( args, fmt );
	    vsnprintf( buffer_, N, fmt, args );
        va_end( args );
    }

    ~BufferRecord() { push(); }

    void getMessage(std::string& result) const  { result = buffer_; }

protected:
    char buffer_[N];
};



//-----------------------------------------------------------------------------
// class PersistentRecord
//-----------------------------------------------------------------------------
//
class PersistentRecord : public RecordBase
{
public:
    PersistentRecord() : RecordBase() {}
    PersistentRecord(const RecordBase& rhs);

    const char* getFile() const                 { return strFile_.c_str(); }
    const char* getFunction() const             { return strFunction_.c_str(); }

    void getMessage(std::string& result) const  { result = strMessage_; }

protected:
    std::string strMessage_;
    std::string strFile_;
    std::string strFunction_;
};


}} // namespace e3::log

