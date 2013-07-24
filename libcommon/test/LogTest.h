//-------------------------------------------------------------------
// LogTest.h
//
// Test classes for e3::log
//-------------------------------------------------------------------

#pragma once

#include <gtest/gtest.h>


namespace e3 { namespace log {

//--------------------------------------------------------
// class LogTest
//--------------------------------------------------------
//
class LogTest: public ::testing::Test 
{ 
public: 
    LogTest() {};
    ~LogTest() {}

    bool testBasicLogger();
    bool testPriority();
    bool testFileSink();

    bool testSinkSet();
};



}} // namespace e3::log


