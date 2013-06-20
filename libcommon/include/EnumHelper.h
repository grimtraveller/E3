
#pragma once

#include <map>
#include <stdexcept>
#include <boost/bimap.hpp>



//----------------------------------------------------------------------------------
// class EnumMapper
// A bidirectional map that bridges between constants defined in different libraries
//----------------------------------------------------------------------------------

class EnumException : public std::runtime_error {
public:
    EnumException() : std::runtime_error("") {};
};


template<typename TLeft, typename TRight>
class EnumMapper : public boost::bimaps::bimap<TLeft, TRight>
{
public:
    void add(TLeft left, TRight right)    
    { 
        insert(value_type(left, right)); 
    }


    TLeft getLeft(TRight keyRight) const       
    { 
        right_const_iterator pos = right.find(keyRight);
        if(pos != right.end()) {
            return pos->second;
        }
        throw EnumException();
    }

    TRight getRight(TLeft keyLeft) const       
    { 
        left_const_iterator pos = left.find(keyLeft);
        if(pos != left.end()) {
            return pos->second;
        }
        throw EnumException();
    }
};


//----------------------------------------------------------------------------------
// class EnumNames
//
// Maps enum values to strings. Furthermore a description can be assigned 
// to each value.
//----------------------------------------------------------------------------------

template<typename TEnum>
class EnumNames
{
public:
    typedef std::map<typename TEnum, std::string> NameMap;
    typename typedef NameMap::const_iterator ConstIterator;

protected:
    NameMap shortNames_;
    NameMap longNames_;

public:
    void add(TEnum value, const std::string& shortName, const std::string& longName="")    
    { 
        std::pair<NameMap::iterator, bool> result1, result2;

        result1 = shortNames_.insert(NameMap::value_type(value, shortName));
        result2 = longNames_.insert(NameMap::value_type(value, longName));

        if(result1.second == false || result2.second == false) 
            throw EnumException();
    }

    const TEnum getValue(const std::string& shortName) const   
    { 
        for (NameMap::const_iterator it = shortNames_.begin(); it != shortNames_.end(); ++it)
        {
            if (it->second == shortName)
            {
                TEnum key = it->first;
                return key;
            }
        }   
        throw EnumException();
    }

    const std::string& getShortName(TEnum value) const { return findName(value, shortNames_); }    
    const std::string& getLongName(TEnum value) const  { return findName(value, longNames_); }
    ConstIterator begin() const                        { return shortNames_.begin(); }  // TODO: return iterator only over keys, not values
    ConstIterator end() const                          { return shortNames_.end(); }

protected:
    const std::string& findName(TEnum value, const NameMap& map) const          
    { 
        NameMap::const_iterator pos = map.find(value);
        if (pos == map.end())
            throw EnumException();

        return pos->second; 
    }
};

