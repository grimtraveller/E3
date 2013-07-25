//-------------------------------------------------------------------
// LibCommonTest.h
//
// Test classes for e3::common
//-------------------------------------------------------------------

#pragma once

#include <gtest/gtest.h>


namespace e3 { namespace common {

//--------------------------------------------------------
// class UtilitiesTest
//--------------------------------------------------------
//
class UtilitiesTest: public ::testing::Test 
{ 
public: 
    bool testRandomString();
};


//--------------------------------------------------------
// class PreallocatedQueueTest
//--------------------------------------------------------
//
class PreallocatedQueueTest: public ::testing::Test 
{ 
public: 
    bool testAll();
};





}} // namespace e3::common


