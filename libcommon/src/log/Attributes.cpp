
//--------------------------------------------------------
// Attributes.cpp
//--------------------------------------------------------

#include <boost/functional/hash.hpp>
#include <log/Attributes.h>
#include <log/Log.h>


namespace e3 { namespace log {


size_t Attribute::hash(const std::string& s)
{
    boost::hash<std::string> hashFunction;
    std::size_t result = hashFunction(s);

    return result;
}



//-----------------------------------------------------------------------------------------
// class InternalAttribute
//-----------------------------------------------------------------------------------------
//
void InternalAttribute::realize(const RecordBase& record, std::ostringstream& os) const
{
    switch(type_) {
    case Message:  
        {
        std::string msg;
        record.getMessage(msg);
        os << msg; 
        break;
        }
    case Priority:     record.getPriorityAsString(os); break;
    case FileName:     os << record.getFileName(); break;
    case FunctionName: os << record.getFunctionName(); break;
    case LineNum:      os << record.getLineNum(); break;
    case LineBreak:    os << std::endl; break;
    case Tab:          os << '\t'; break;

    default: ASSERT(false);
    }
}


//-----------------------------------------------------------------------------------------
// class TextAttribute
//-----------------------------------------------------------------------------------------
//
void TextAttribute::realize(const RecordBase& record, std::ostringstream& os) const
{
    os << text_;
}

	
	
}} // namespace e3::log