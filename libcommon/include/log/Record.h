
//------------------------------------------------------------
// Record.h
//
// Stores a single log record and environmental data
// gathered from the macros __FILE__, __FUNCTION__ and __LINE__
//------------------------------------------------------------

#pragma once

#include <string>
#include <sstream>


namespace e3 { namespace log {

class Record
{
public:
    Record(const char* file, const char* function, int line); // TODO: char* won't work when Records are cached
    ~Record();

    std::ostringstream& getStream() { return stream_; }

    std::string getMessage() const  { return stream_.str(); }       // TODO: can these methods return references?
    std::string getFile() const     { return file_; }
    std::string getFunction() const { return function_; }
    std::string getLine() const     { std::ostringstream os; os << line_; return os.str(); }

protected:
    std::ostringstream stream_;

    const char* file_;
    const char* function_;
    int line_;
};

}} // namespace e3::log

