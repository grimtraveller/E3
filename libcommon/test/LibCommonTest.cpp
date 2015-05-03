
//-------------------------------------------------------------------
// LibCommonTest.cpp
//
// Test classes for e3::common
//-------------------------------------------------------------------

#include <string>
#include <set>

#define protected public        // access to protected members of experimentees;

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
}


}} // namespace e3::common

