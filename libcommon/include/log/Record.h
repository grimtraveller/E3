
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
    RecordBase(Logger* logger, int priority, const char* fileName, const char* functionName, int lineNum); 
    virtual ~RecordBase() {}                          
                                                 
    virtual void getMessage(std::string& result) const {}
    virtual int getPriority() const              { return priority_; }
    virtual void getPriorityAsString(std::ostringstream& os) const;
    virtual Logger* getLogger() const            { return logger_; }

    virtual const char* getFileName() const      { return fileName_; }
    virtual const char* getFunctionName() const  { return functionName_; }
    virtual int getLineNum() const               { return lineNum_; }

protected:
    virtual void push();

    Logger* logger_;
    int priority_;

    const char* fileName_;
    const char* functionName_;
    int lineNum_;
};



class StreamRecord : public RecordBase, public boost::noncopyable
{
public:
    StreamRecord(Logger* logger, int priority); 
    StreamRecord(Logger* logger, int priority, const char* fileName, const char* functionName, int lineNum); 
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
    BufferRecord(Logger* logger, int priority, const char* fileName, const char* functionName, int lineNum, const char* fmt, ...) :
        RecordBase(logger, priority, fileName, functionName, lineNum)
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

    const char* getFileName() const             { return strFileName_.c_str(); }
    const char* getFunctionName() const         { return strFunctionName_.c_str(); }

    void getMessage(std::string& result) const  { result = strMessage_; }

protected:
    std::string strMessage_;
    std::string strFileName_;
    std::string strFunctionName_;
};


}} // namespace e3::log

