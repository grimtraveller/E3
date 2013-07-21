
//--------------------------------------------------------
// Attributes.cpp
//--------------------------------------------------------

#include <log/Attributes.h>

namespace e3 { namespace log {


//-----------------------------------------------------------------------------------------
// class MessageAttribute
//-----------------------------------------------------------------------------------------
//
void MessageAttribute::realize(const Record& record, std::ostringstream& os) const
{
    os << record.getMessage();
}


//-----------------------------------------------------------------------------------------
// class InternalAttribute
//-----------------------------------------------------------------------------------------
//
void InternalAttribute::realize(const Record& record, std::ostringstream& os) const
{
    switch(type_) {
    case Message:      os << record.getMessage(); break;
    case FileName:     os << record.getFile(); break;
    case FunctionName: os << record.getFunction(); break;
    case LineNum:      os << record.getLine(); break;
    case LineBreak:    os << std::endl; break;
    case Tab:          os << '\t'; break;

    default: ASSERT(false);
    }
}


//-----------------------------------------------------------------------------------------
// class TextAttribute
//-----------------------------------------------------------------------------------------
//
void TextAttribute::realize(const Record& record, std::ostringstream& os) const
{
    os << text_;
}

	
	
}} // namespace e3::log