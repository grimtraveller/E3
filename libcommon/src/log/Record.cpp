
//------------------------------------------------------------
// Record.cpp
//
// Stores a single log record and environmental data
// gathered from the macros __FILE__, __FUNCTION__ and __LINE__
//------------------------------------------------------------

#include <log/Record.h>
#include <log/Log.h>

namespace e3 { namespace log {


Record::Record(const char* file, const char* function, int line) :
    file_(file),
    function_(function),
    line_(line)
{
    //std::cout << "FILENAME="<<filename<<std::endl;
    //std::cout << "FUNCTIONNAME="<<functionname<<std::endl; 
    //std::cout << "LINENUMBER="<<linenumber<<std::endl;
}


Record::~Record()
{
    stream_.flush();
    Logger* logger = Core::instance().getLogger();
    logger->output(*this);
}


}} // namespace e3::log

