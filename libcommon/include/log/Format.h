
//-----------------------------------------------------------------
// Format.h
//
// Log string formatting
//------------------------------------------------------------------

#pragma once

#include <string>
#include <vector>
#include <utility>      // std::pair


namespace e3 { namespace log {

class Format
{
public:
    enum TokenType { Tag, Text };
    typedef std::pair<TokenType, std::string> Token;
private:
    typedef std::vector<Token> TokenVector;
public:
    typedef TokenVector::const_iterator TokenIterator;

    bool parse(const std::string& formatString);

    TokenIterator begin() const  { return tokens_.begin(); }
    TokenIterator end() const    { return tokens_.end(); }

protected:
    TokenVector tokens_;

    friend struct TokenHandler;
};

}}  // namespace e3::log