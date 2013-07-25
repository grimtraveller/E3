
//-------------------------------------------------------------------
// LibCommonTest.cpp
//
// Test classes for e3::common
//-------------------------------------------------------------------

#include <string>
#include <set>

#define protected public        // access to protected members of experimentees;

#include <Utilities.h>
#include <PreallocatedQueue.h>
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


//--------------------------------------------------------
// class PreallocatedQueueTest
//--------------------------------------------------------
//
bool PreallocatedQueueTest::testAll()
{
    bool result = true;
    
    PreallocatedQueue<int> queue(10000, 1);
    result &= (queue.size() == 10000);
    result &= queue.available() == 10000;

    for(unsigned i=0; i<10000; i++)
    {
        queue.push(i);
        result &= queue.front_ == i+1;
        result &= queue.available() == 10000-i-1;
    }
    result &= queue.empty() == false;

    queue.resize(1);
    result &= queue.front() == 0;
    result &= (queue.size() == 1);
    result &= queue.front_ == 1;
    result &= queue.available() == 0;

    queue.resize(0);
    result &= queue.empty();
    result &= queue.front_ == 0;
    result &= queue.available() == 0;

    queue.push(1);
    result &= queue.front_ == 1;
    result &= queue.front() == 1;
    result &= queue.available() == 0;

    queue.push(2);                  // this must cause an autoresize
    result &= queue.size() == 2;    // size before: 1, autoresize: 1
    result &= queue.front_ == 2;
    result &= queue.front() == 2;
    result &= queue.available() == 0;

    int n = queue.front();
    result &= n == 2;
    
    queue.pop();
    result &= queue.front_ == 1;
    n = queue.front();
    result &= n == 1;
    result &= queue.available() == 1;

    queue.pop();
    result &= queue.empty();
    result &= queue.available() == 2;
    result &= queue.front_ == 0;

    try {
        n = queue.front();   // must throw
        return false;
    }
    catch(const std::exception&) {}

    try {
        queue.pop();   // must throw
        return false;
    }
    catch(const std::exception&) {}

    queue.resize(10);
    result &= queue.available() == 10;

    return result;
}




}} // namespace e3::common

