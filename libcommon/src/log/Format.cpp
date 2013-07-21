
//--------------------------------------------------
// Format.cpp
// 
// Log string formatting
//--------------------------------------------------

#include <boost/spirit/include/qi.hpp>      // compiles slow

#include <log/Format.h>


namespace e3 { namespace log {

namespace qi = boost::spirit::qi;

struct TokenHandler
{
    void operator()(const std::string& token, qi::unused_type, qi::unused_type) const 
    {
        //std::cout << "Tag: <" << str << ">   length="<<str.length() << std::endl;
        tokens_.push_back(std::make_pair(type_, token));
    }

    TokenHandler(Format::TokenType type, Format::TokenVector& tokens) : 
        type_(type),
        tokens_(tokens)
        {}

    Format::TokenType type_;
    Format::TokenVector& tokens_;
};


bool Format::parse(const std::string& formatString)
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
    catch(const qi::expectation_failure<IteratorT>& e) {
        //EXCEPTION(std::exception, "Error parsing log format: missing closing %-tag");
        throw std::exception("Error parsing log format: unbalanced %-tags");
    }
        
    //std::cout << "parseTags " << (result?"successful":"failed") << std::endl;
    //std::cout << std::endl;
    //return result;
}

}} // namespace e3::log