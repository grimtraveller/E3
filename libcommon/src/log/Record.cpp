
//------------------------------------------------------------
// Record.cpp
//
// Stores a single log record and environmental data
// gathered from the macros __FILE__, __FUNCTION__ and __LINE__
//------------------------------------------------------------

#include <log/Record.h>
#include <log/Log.h>


namespace e3 { namespace log {



RecordBase::RecordBase(Logger* logger, int priority) :
    logger_(logger),
    priority_(priority),
    file_(NULL),
    function_(NULL),
    line_(0)
    {}



RecordBase::RecordBase(Logger* logger, int priority, const char* file, const char* function, int line) :
    logger_(logger),
    priority_(priority),
    file_(file),
    function_(function),
    line_(line)
    {}


RecordBase::RecordBase(const RecordBase& rhs) :
    logger_(rhs.logger_),
    priority_(rhs.priority_),
    file_(rhs.file_),
    function_(rhs.function_),
    line_(rhs.line_)
    {}



void RecordBase::push()
{
    if(logger_) {
        logger_->pushRecord(*this); 
    }
}




//-----------------------------------------------------------------------------
// class StreamRecord
//-----------------------------------------------------------------------------
//
StreamRecord::StreamRecord(Logger* logger, int priority) :
    RecordBase(logger, priority)
    {}


StreamRecord::StreamRecord(Logger* logger, int priority, const char* file, const char* function, int line) :
    RecordBase(logger, priority, file, function, line)
    {}




//-----------------------------------------------------------------------------
// class BufferRecord
//-----------------------------------------------------------------------------
//
//BufferRecord::BufferRecord(Logger* logger, int priority, const char* file, const char* function, int line, const char* fmt, ...) :
//    RecordBase(logger, priority, file, function, line)
//{
//	va_list args;
//	va_start( args, fmt );
//	vsprintf( buffer_, fmt, args );
//    va_end( args );
//}



//-----------------------------------------------------------------------------
// class PersistentRecord
//-----------------------------------------------------------------------------
//
PersistentRecord::PersistentRecord(const RecordBase& rhs) :
    RecordBase(rhs)
{
    rhs.getMessage(strMessage_);
    
    if(file_) strFile_         = rhs.getFile();
    if(function_) strFunction_ = rhs.getFunction();

    line_ = rhs.getLine();
}


//std::ostringstream& operator<<(std::ostringstream& os, const PersistentRecord& record)
//{
//    os << (const_cast<PersistentRecord&>(record)).strMessage_;
//    return os;
//}


}} // namespace e3::log

