
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
    fileName_(NULL),
    functionName_(NULL),
    lineNum_(0)
    {}



RecordBase::RecordBase(Logger* logger, int priority, const char* fileName, const char* functionName, int lineNum) :
    logger_(logger),
    priority_(priority),
    fileName_(fileName),
    functionName_(functionName),
    lineNum_(lineNum)
    {}


RecordBase::RecordBase(const RecordBase& rhs) :
    logger_(rhs.logger_),
    priority_(rhs.priority_),
    fileName_(rhs.fileName_),
    functionName_(rhs.functionName_),
    lineNum_(rhs.lineNum_)
    {}



void RecordBase::push()
{
    if(logger_) {
        logger_->pushRecord(*this); 
    }
}


void RecordBase::getPriorityAsString(std::ostringstream& os) const
{
    if(logger_) {
        logger_->priorityToString(os, priority_);
    }
}




//-----------------------------------------------------------------------------
// class StreamRecord
//-----------------------------------------------------------------------------
//
StreamRecord::StreamRecord(Logger* logger, int priority) :
    RecordBase(logger, priority)
    {}


StreamRecord::StreamRecord(Logger* logger, int priority, const char* fileName, const char* functionName, int lineNum) :
    RecordBase(logger, priority, fileName, functionName, lineNum)
    {}




//-----------------------------------------------------------------------------
// class PersistentRecord
//-----------------------------------------------------------------------------
//
PersistentRecord::PersistentRecord(const RecordBase& rhs) :
    RecordBase(rhs)
{
    rhs.getMessage(strMessage_);
    
    if(fileName_) strFileName_         = rhs.getFileName();
    if(functionName_) strFunctionName_ = rhs.getFunctionName();
}


PersistentRecord::PersistentRecord(const PersistentRecord& rhs) :
    RecordBase(rhs)
{
#ifdef _DEBUG
    //throw std::exception("should never be called");
#endif

    strMessage_.reserve(255);
    strFileName_.reserve(16);
    strFunctionName_.reserve(16);
    
    strMessage_      = rhs.strMessage_;
    strFileName_     = rhs.strFileName_;
    strFunctionName_ = rhs.strFunctionName_;
}


}} // namespace e3::log

