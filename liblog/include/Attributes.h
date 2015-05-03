
//--------------------------------------------------------
// Attributes.h
//--------------------------------------------------------

#pragma once

#include <string>
#include <sstream>
#include <map>

#include <boost/shared_ptr.hpp>

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

    virtual void realize(const RecordBase& record, std::ostringstream& os) const = 0;

    static size_t hash(const std::string& s);
};


typedef boost::shared_ptr<Attribute> AttributePtr;
typedef std::map<size_t, AttributePtr > AttributeMap;



//-----------------------------------------------------------------------------------------
// class InternalAttribute
//-----------------------------------------------------------------------------------------
//
class InternalAttribute : public Attribute
{
public:
    enum Type { 
        Message, 
        Priority,
        FileName, 
        FunctionName, 
        LineNum, 
        LineBreak, 
        Tab 
    };
    InternalAttribute(Type type) : Attribute(), type_(type) {}

    void realize(const RecordBase& record, std::ostringstream& os) const;

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

    void realize(const RecordBase& record, std::ostringstream& os) const;

private:
    std::string text_;
};






// TODO: 
// timeline attribute
// timestamp attribute
// counter attribute
// priority attribute

	
}} // namespace e3::log