
//--------------------------------------------------
// Format.cpp
// 
// Log string formatting
//--------------------------------------------------

#include <vector>
#include <utility>      // std::pair

#include <boost/any.hpp>
#include <boost/spirit/include/qi.hpp>      // compiles slow

#include <log/Format.h>


namespace e3 { namespace log {


//------------------------------------------------------------
// class Format::Impl
//------------------------------------------------------------
//
class Format::Impl
{
public:
    friend class Format;

    Impl() {}
    Impl(const Impl& other) { tokens_ = other.tokens_; }
    Impl(Impl* other)       { tokens_ = other->tokens_; }


    bool parse(const std::string& formatString);
    void realize(const AttributeMap& attributes,  const RecordBase& record, std::ostringstream& os) const;

protected:
    enum TokenType { Tag, Text };
    typedef std::pair<TokenType, boost::any> Token;
    typedef std::vector<Token> TokenVector;

    TokenVector tokens_;

    friend struct TokenHandler;  // callback for the parser
};


//------------------------------------------------------------------
// struct TokenHandler
//------------------------------------------------------------------
//

namespace qi = boost::spirit::qi;

// Parser callback
struct TokenHandler
{
    
    void operator()(const std::string& token, qi::unused_type, qi::unused_type) const 
    {
        if(type_ == Format::Impl::Tag) {
            size_t hash = Attribute::hash(token);
            tokens_.push_back(std::make_pair(type_, hash));
        }
        else {
            tokens_.push_back(std::make_pair(type_, token));
        }
    }

    TokenHandler(Format::Impl::TokenType type, Format::Impl::TokenVector& tokens) : 
        type_(type),
        tokens_(tokens)
        {}

    Format::Impl::TokenType type_;
    Format::Impl::TokenVector& tokens_;
};




bool Format::Impl::parse(const std::string& formatString)
{
    tokens_.clear();

    TokenHandler tagHandler(Tag, tokens_);
    TokenHandler textHandler(Text, tokens_);

    typedef std::string::const_iterator IteratorT;
    typedef qi::rule<IteratorT, std::string()> StringRuleT;
    typedef qi::rule<IteratorT, char()> CharRuleT;
        
    CharRuleT escChar   = '\\' >> qi::char_("\\%"); 
    StringRuleT tag     = '%' > +(qi::char_ - '%') > '%';
    StringRuleT text    = +(escChar | ~qi::char_('%'));
    StringRuleT grammar = *(text[textHandler] | tag[tagHandler]); 

    try {
        bool result = qi::parse(formatString.begin(), formatString.end(), grammar);
        return result;
    }
    catch(const qi::expectation_failure<IteratorT>&) {
        throw std::exception("Error parsing log format: unbalanced %-tags");
    }
}



void Format::Impl::realize(const AttributeMap& attributes, const RecordBase& record, std::ostringstream& os) const
{
    for(TokenVector::const_iterator itFormat=tokens_.begin(); itFormat!=tokens_.end(); ++itFormat)
    {
        const Token& token     = *itFormat;
        TokenType type         = token.first;
        const boost::any& data = token.second;

        switch(type) 
        {
        case Tag:
            {
                size_t hash = boost::any_cast<size_t>(data);
                AttributeMap::const_iterator pos = attributes.find(hash);

                if(pos != attributes.end())
                {
                    Attribute* attribute = (*pos).second.get();
                    ASSERT(attribute);
                    attribute->realize(record, os);
                }
            }
            break;

        case Text:
            {
                const std::string& s = boost::any_cast<std::string>(data);
                os << s;
            }
            break;

        default: ASSERT(false);
        }
    }
}



//-------------------------------------------------------------
// class Format
//-------------------------------------------------------------
//
Format::Format() : pimpl_(NULL)
{
    pimpl_ = new Impl();
}



Format::Format(const Format& other) : pimpl_(NULL)
{
    pimpl_ = new Impl(other.pimpl_);
}



Format& Format::operator=(const Format& other)
{
    delete pimpl_;
    pimpl_ = new Impl(other.pimpl_);

    return *this;
}



Format::~Format()
{
    if(pimpl_) delete pimpl_;
}


bool Format::parse(const std::string& formatString)                          
{ 
    return pimpl_->parse(formatString);
}



void Format::realize(const AttributeMap& attributes, const RecordBase& record, std::ostringstream& os) const
{ 
    pimpl_->realize(attributes, record, os); 
}


}} // namespace e3::log