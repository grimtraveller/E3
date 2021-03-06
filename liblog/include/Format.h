
//-----------------------------------------------------------------
// Format.h
//
// Log string formatting
//------------------------------------------------------------------

#pragma once

#include <string>
#include <sstream>

//#include <boost/utility.hpp>  // boost::noncopyable

#include <log/Record.h>
#include <log/Attributes.h>


namespace e3 { namespace log {

class Format
{
public:
    Format();
    Format(const Format& rhs);
    Format& operator=(const Format& rhs);
    ~Format();

    bool parse(const std::string& formatString);
    void realize(const AttributeMap& attributes,  const RecordBase& record, std::ostringstream& os) const;

protected:
    friend struct TokenHandler;  // callback for the parser

    class Impl;
    Impl* pimpl_;
};

}}  // namespace e3::log