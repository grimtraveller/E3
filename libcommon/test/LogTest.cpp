
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


bool LogTest::testBasicLogger()
{
    Priority priority = Verbose1;
    Logger logger(priority);
    NullSink* sink = new NullSink();
    logger.addSink(sink);
    Core::instance().setLogger(&logger);

    int n = 123;
    std::ostringstream os;
    os << "BasicLoggerTest " << n << std::endl; 
    const std::string& s1 = os.str();          // create a reference string

    LOG(priority) << "BasicLoggerTest " << n;
    const std::string& s2 = sink->bin_;

    return s1 == s2;
}


bool LogTest::testPriority()
{
    bool result = true;

    Logger logger(Error);
    NullSink* sink = new NullSink();
    logger.addSink(sink);
    Core::instance().setLogger(&logger);

    LOG(Verbose1) << "abc";
    result &= (sink->bin_.length() == 0);

    LOG(Error) << "abc";

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
        Logger logger(Verbose1);
        FileSink* sink = new FileSink(filename);
        logger.addSink(sink);
        Core::instance().setLogger(&logger);

        LOG(Verbose1) << testString;
        Core::instance().setLogger(NULL);
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
    
    Logger logger;
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

