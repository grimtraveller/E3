
//--------------------------------------------------------
// Attributes.h
//--------------------------------------------------------

#pragma once

#include <string>
#include <CommonMacros.h>

namespace e3 { namespace log {
	
//-----------------------------------------------------------------------------------------
// class Attribute
// Interface for log attributes. These are configurable token to be inserted to LogRecords
//-----------------------------------------------------------------------------------------
//
class Attribute
{
public:
    class Impl
    {
    public:
        virtual const std::string& getString(const LogRecord& record) const = 0;
    };

    Attribute() : impl_(NULL)           {}
    Attribute(Impl* impl) : impl_(impl) {}
    ~Attribute()                        {}

    virtual const std::string& getString() const
    { 
        ASSERT(impl_);
        return impl_->getString(); 
    }

    //friend std::ostream& operator<< (std::ostream& out, Attribute& attr);
protected:
    Impl* impl_;  // TODO: make this intrusive_ptr
};



//-----------------------------------------------------------------------------------------
// class InternalAttribute
//-----------------------------------------------------------------------------------------
//


//-----------------------------------------------------------------------------------------
// class TextAttribute
//-----------------------------------------------------------------------------------------
//
class TextAttribute : public Attribute
{
public:
    TextAttribute(const std::string& text="") : 
        Attribute(new Impl(text)) 
    {}

protected:
    class Impl : public Attribute::Impl
    {
    public:
        Impl(const std::string& text) : text_(text)                  {}
        const std::string& getString(const LogRecord& record) const  { return text_; }

    protected:
        std::string text_;
    };
};






// TODO: 
// file attribute
// line attribute
// timeline attribute
// timestamp attribute
// counter attribute

	
}} // namespace e3::log