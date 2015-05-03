//-------------------------------------------------------------------
// LogTest.h
//
// Test classes for e3::log
//-------------------------------------------------------------------

#pragma once

#include <gtest/gtest.h>

//void* operator new (std::size_t size) throw (std::bad_alloc);


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
    bool testLocking();

    bool testSinkSet();
};



//--------------------------------------------------------
// class RecordBufferTest
//--------------------------------------------------------
//
class RecordBufferTest: public ::testing::Test 
{ 
public: 
    bool testBasic();
    bool testLoop();
    bool testOverflowUseOldest();
    bool testOverflowUseNewest();
    bool testOverflowResize();
    bool testOverflowThrow();
};


}} // namespace e3::log


