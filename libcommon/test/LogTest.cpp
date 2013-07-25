
//-------------------------------------------------------------------
// LogTest.cpp
//
// Test classes for e3::log
//-------------------------------------------------------------------

#include <log/Log.h>
#include "LogTest.h"

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

}} // namespace e3::log

