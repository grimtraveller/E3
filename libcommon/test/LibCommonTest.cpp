
//-------------------------------------------------------------------
// LibCommonTest.cpp
//
// Test classes for e3::common
//-------------------------------------------------------------------

#include <string>
#include <set>

#include <Utilities.h>
#include "LibCommonTest.h"

namespace e3 { namespace common {


//--------------------------------------------------------
// class UtilitiesTest
//--------------------------------------------------------
//
bool UtilitiesTest::testRandomString()
{
    unsigned stringLength = 3;
    std::string s = randomString(stringLength);

    return s.length() == stringLength;
    
    //bool result = true;
    //unsigned stringLength  = 3;
    //unsigned numIterations = 27;

    //typedef std::set<std::string> StringSet;
    //StringSet container;
    //
    //for(unsigned i=0; i<numIterations; i++)
    //{
    //    std::string s = randomString(stringLength, "abc");
    //    std::pair<StringSet::iterator, bool> p = container.insert(s);
    //    result &= p.second;

    //    if(p.second == false) {
    //        int x = 0;
    //    }
    //}
    //result &= container.size() == numIterations;

    //return result;
}

}} // namespace e3::common

