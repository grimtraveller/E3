
//-------------------------------------------------------------------
// LogTest.cpp
//
// Test classes for e3::log
//-------------------------------------------------------------------

//#define protected public        // access to protected members of experimentees;


#include <log/RecordBuffer.h>
#include <log/Log.h>

#include "LogTest.h"


//void* operator new (std::size_t size) throw (std::bad_alloc)
//{
//  // add tracking code here...
//  return malloc (size);
//}


namespace e3 { namespace log {

//--------------------------------------------------------
// class LogTest
//--------------------------------------------------------
//
struct NullSink : public Sink
{
    NullSink() : Sink() {}
    
    void output(const std::string& msg) { bin_ += msg; }
    std::string bin_;
};



enum TestPriority
{
    Low,
    High,
    Sick
};



std::ostringstream& operator<< (std::ostringstream& stream, TestPriority priority)
{
    static const char* strings[] =
    {
        "Low",
        "High", 
        "Sick"
    };

    if (static_cast< std::size_t >(priority) < sizeof(strings) / sizeof(*strings))
        stream << strings[priority];
    else
        stream << static_cast< int >(priority);

    return stream;
}





bool LogTest::testBasicLogger()
{
    Priority priority = DEBUG;
    PriorityLogger<> logger(priority);
    NullSink* sink = new NullSink();
    logger.addSink(sink);

    int n = 123;
    std::ostringstream os;
    os << "BasicLoggerTest " << n << std::endl; 
    const std::string& s1 = os.str();          // create a reference string

    E3_LOG_PRIO(priority, logger) << "BasicLoggerTest " << n;
    const std::string& s2 = sink->bin_;

    return s1 == s2;
}



bool LogTest::testPriority()
{
    bool result = true;

    PriorityLogger<TestPriority> logger(Sick);
    NullSink* sink = new NullSink();
    logger.addSink(sink);

    E3_LOG_PRIO(Low, logger) << "abc";
    result &= (sink->bin_.length() == 0);

    E3_LOG_PRIO(Sick, logger) << "abc";

    result &= (sink->bin_.length() > 0);    

    return result;
}



bool LogTest::testFileSink()
{
    std::string filename   = "playertest.log";
    std::string testString = "abc";

    std::ifstream ifs(filename.c_str());
    if (ifs.good()) {
        ifs.close();
        return false;       // test file exists, that should not be
    }

    {
        PriorityLogger<> logger(DEBUG);
        FileSink* sink = new FileSink(filename);
        logger.addSink(sink);

        E3_LOG_PRIO(DEBUG, logger) << testString;
    }

    ifs.open(filename, std::ofstream::in);
    if(ifs.is_open() == false) 
        return false;

    std::stringstream buffer;
    buffer << ifs.rdbuf();
    std::string content = buffer.str();

    if(ifs.is_open()) 
        ifs.close();
    remove(filename.c_str());

    std::ostringstream os;
    os << testString << std::endl;
    testString = os.str();

    return content == testString;
}


bool LogTest::testSinkSet()
{
    bool result = true;

    StreamSink* sink1 = new StreamSink(NULL);
    StreamSink* sink2 = new StreamSink(NULL);
    
    PriorityLogger<> logger(DEBUG);
    const SinkSet& sinks = logger.getSinks();
    
    result &= logger.addSink(sink1);
    result &= sinks.size() == 1;
    result &= !(logger.addSink(sink1));     // should be rejected
    result &= sinks.size() == 1;
    result &= logger.addSink(sink2);
    result &= sinks.size() == 2;

    logger.removeSink(sink1);
    result &= sinks.size() == 1;

    logger.clearSinks();
    result &= sinks.size() == 0;

    return result;
}



bool LogTest::testLocking()
{
    Priority priority = DEBUG;
    PriorityLogger<> logger(priority);
    NullSink* sink = new NullSink();
    logger.setOverflowPolicy(ON_OVERFLOW_RESIZE);
    logger.addSink(sink);
    logger.lock();

    for(unsigned i=0; i<10; i++) {
        E3_LOG_PRIO_BUF(priority, logger, "%d", i);
    }

    logger.flush();

    std::ostringstream os;
    for(unsigned i=0; i<10; i++)
    {
        os << i << std::endl;
    }
    const std::string& reference = os.str();   
    bool result = sink->bin_ == reference;

    return result;
}



//--------------------------------------------------------
// class RecordBufferTest
//--------------------------------------------------------
//
bool RecordBufferTest::testBasic()
{
    bool result = true;
    RecordBuffer<int> buffer(3);

    result &= buffer.empty()     == true;
    result &= buffer.full()      == false;
    result &= buffer.size()      == 3;
    result &= buffer.available() == 3;

    buffer.push(1);
    result &= buffer.empty()     == false;
    result &= buffer.full()      == false;
    result &= buffer.size()      == 3;
    result &= buffer.available() == 2;

    buffer.push(2);
    result &= buffer.empty()     == false;
    result &= buffer.full()      == false;
    result &= buffer.size()      == 3;
    result &= buffer.available() == 1;

    buffer.push(3);
    result &= buffer.empty()     == false;
    result &= buffer.full()      == true;
    result &= buffer.size()      == 3;
    result &= buffer.available() == 0;

    int n1 = buffer.pop();
    result &= n1 == 1;
    result &= buffer.full()      == false;
    result &= buffer.empty()     == false;
    result &= buffer.available() == 1;

    int n2 = buffer.pop();
    result &= n2 == 2;
    result &= buffer.full()      == false;
    result &= buffer.empty()     == false;
    result &= buffer.available() == 2;

    int n3 = buffer.pop();
    result &= n3 == 3;
    result &= buffer.full()      == false;
    result &= buffer.empty()     == true;
    result &= buffer.available() == 3;

    try {                               // expecting underflow
        int n4 = buffer.pop();
        return false;
    }
    catch(const UnderflowException&) {}

    buffer.push(1);
    buffer.clear();
    result &= buffer.empty() == true;

    return result;
}



bool RecordBufferTest::testLoop()
{
    bool result = true;
    size_t bufferSize = 3;
    RecordBuffer<int> buffer(bufferSize);

    for(unsigned i=0; buffer.full()==false; i++)
    {
        buffer.push(i);
        result &= buffer.available() == bufferSize - i - 1;
    }

    for(unsigned i=0; buffer.empty()==false; i++)
    {
        result &= buffer.pop() == i;
    }
    return result;
}



bool RecordBufferTest::testOverflowUseOldest()
{
    bool result = true;
    size_t bufferSize = 3;
    RecordBuffer<int> buffer(bufferSize);
    buffer.setOverflowPolicy(ON_OVERFLOW_USE_OLDEST);

    buffer.push(1);
    buffer.push(2);
    buffer.push(3);   
    result &= buffer.full();
    buffer.push(4);
    buffer.push(5);
    buffer.push(6);

    result &= buffer.pop() == 1;
    result &= buffer.pop() == 2;
    result &= buffer.pop() == 3;
    result &= buffer.empty();

    try {
        buffer.pop();
        return false;
    }
    catch(const UnderflowException&) {}

    return result;
}



bool RecordBufferTest::testOverflowUseNewest()
{
    bool result = true;
    size_t bufferSize = 3;
    RecordBuffer<int> buffer(bufferSize);
    buffer.setOverflowPolicy(ON_OVERFLOW_USE_NEWEST);

    buffer.push(1);
    buffer.push(2);
    buffer.push(3);   
    result &= buffer.full();
    buffer.push(4);
    result &= buffer.full() == false;
    buffer.push(5);
    result &= buffer.full() == false;
    buffer.push(6);
    result &= buffer.full();

    result &= buffer.pop() == 4;
    result &= buffer.pop() == 5;
    result &= buffer.pop() == 6;
    result &= buffer.empty();

    return result;
}



bool RecordBufferTest::testOverflowResize()
{
    bool result = true;
    size_t bufferSize = 3;
    RecordBuffer<int> buffer(bufferSize);
    buffer.setOverflowPolicy(ON_OVERFLOW_RESIZE);

    buffer.push(1);
    buffer.push(2);
    buffer.push(3);   
    buffer.push(4);     // should resize to 4
    result &= buffer.full();
    result &= buffer.size() == 4;
    
    buffer.push(5);     // should resize to 6
    result &= buffer.full() == false;
    buffer.push(6);
    result &= buffer.full();

    result &= buffer.pop() == 1;
    result &= buffer.pop() == 2;
    result &= buffer.pop() == 3;
    result &= buffer.pop() == 4;
    result &= buffer.pop() == 5;
    result &= buffer.pop() == 6;
    result &= buffer.empty();

    return result;
}



bool RecordBufferTest::testOverflowThrow()
{
    size_t bufferSize = 3;
    RecordBuffer<int> buffer(bufferSize);
    buffer.setOverflowPolicy(ON_OVERFLOW_THROW);

    while(buffer.full() == false)
    {
        buffer.push(17);
    }
    bool result = buffer.full();

    try {
        buffer.push(18);
        return false;
    }
    catch(const OverflowException&) {}

    return result;
}






}} // namespace e3::log

