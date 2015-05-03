

#include <gtest/gtest.h>
#include <string>
#include <map>
#include <thread>
#include <algorithm>

#include "e3_CommonMacros.h"
#include "e3_Exception.h"
#include "e3_Trace.h"
#include "e3_Utilities.h"
#include "e3_Math.h"

#include "LibCommon_BufferTest.inc"
#include "LibCommon_ProfilerTest.inc"
#include "LibCommon_MathTest.inc"


namespace e3 {
    namespace common {
        namespace test {


            TEST(UtilitiesTest, RandomString_Length)
            {
                unsigned length = 3;
                std::string s = RandomString(length);

                EXPECT_EQ(s.size(), length);
            }

            TEST(UtilitiesTest, RandomString_Unique)
            {
                size_t length = 1000;
                std::string s1 = RandomString(1000);
                std::string s2 = RandomString(1000);

                EXPECT_NE(s1, s2);
            }

            TEST(UtilitiesTest, Format_ResultCorrect)
            {
                int length = 1025;
                std::string fmt = "";
                fmt.resize(length);

                // render a long string
                for (int i = 0; i < length; i++) {
                    fmt[i] = 'x';
                }

                std::string result;
                const char* s = fmt.c_str();
                format(result, "%s%s", s, s);  // double the string to exceed the buffer size in e3::common::format

                EXPECT_EQ(result, fmt + fmt);
            }


            TEST(UtilitiesTest, HexToInt)
            {
                typedef std::map<int, std::string> TestMap;
                TestMap m = { 
                    { 0, "0000" }, 
                    { 1, "00000001" }, 
                    { 10, "a" }, 
                    { 255, "ff" },
                    { 255, "FF" },
                    { 11259375, "abcdef" },
                    { 4294967295, "FFFFFFFF" }
                };

                for (TestMap::iterator it = m.begin(); it != m.end(); ++it)
                {
                    EXPECT_EQ(it->first, HexToInt(it->second));
                }
            }


            TEST(ThrowTest, MessageCorrect)
            {
                const char* message = "abcdef";
                try {
                    THROW(std::exception, message);
                }
                catch (const std::exception& e) {
                    const char* p = strstr(e.what(), "abcdef");
                    EXPECT_NE(nullptr, p);
                }
            }
        }
    }
} // namespace e3::common::test
