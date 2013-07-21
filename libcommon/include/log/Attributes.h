
//--------------------------------------------------------
// Attributes.h
//--------------------------------------------------------

#pragma once

#include <string>
#include <sstream>

#include <CommonMacros.h>
#include <log/Record.h>

namespace e3 { namespace log {
	
//-----------------------------------------------------------------------------------------
// class Attribute
// Interface for log attributes. 
// Log attributes are configurable tokens that will be inserted to the output
// of a log record.
//-----------------------------------------------------------------------------------------
//
class Attribute
{
public:
    virtual ~Attribute() {}

    virtual void realize(const Record& record, std::ostringstream& os) const = 0;
};



//-----------------------------------------------------------------------------------------
// class MessageAttribute
//-----------------------------------------------------------------------------------------
//
class MessageAttribute : public Attribute
{
public:
    void realize(const Record& record, std::ostringstream& os) const;
};



//-----------------------------------------------------------------------------------------
// class InternalAttribute
//-----------------------------------------------------------------------------------------
//
class InternalAttribute : public Attribute
{
public:
    enum Type { 
        Message, 
        FileName, 
        FunctionName, 
        LineNum, 
        LineBreak, 
        Tab 
    };
    InternalAttribute(Type type) : Attribute(), type_(type) {}

    void realize(const Record& record, std::ostringstream& os) const;

protected:
    Type type_;
};



//-----------------------------------------------------------------------------------------
// class TextAttribute
//-----------------------------------------------------------------------------------------
//
class TextAttribute : public Attribute
{
public:
    TextAttribute(const std::string& text="") : 
        Attribute(),
        text_(text)
    {}

    void realize(const Record& record, std::ostringstream& os) const;

private:
    std::string text_;
};






// TODO: 
// file attribute
// line attribute
// timeline attribute
// timestamp attribute
// counter attribute

	
}} // namespace e3::log