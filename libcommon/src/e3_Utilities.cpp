
//---------------------------------------------------------------------
// Utilities.cpp
//
// A collection of common tools
//---------------------------------------------------------------------

#include <cstdlib>
#include <time.h> 
#include <cstdarg>
#include <sstream>
#include <chrono>
#include <thread>

#include <e3_Utilities.h>


namespace e3 {
    namespace common {

        inline int DoubleToInt(double d)
        {
            d += 6755399441055744.0;
            return reinterpret_cast<int&>(d);
        }


        void InitRandom()
        {
            srand((unsigned)time(NULL));
        }



        std::string RandomString(int length, const std::string& charset)
        {
            std::string result = "";
            result.resize(length);

            for (int i = 0; i < length; i++) {
                result[i] = charset[rand() % charset.length()];
            }
            return result;
        }


        void vformat(std::string& result, const char* fmt, va_list args)
        {
            // Allocate a buffer on the stack that's big enough for us almost
            // all the time.  Be prepared to allocate dynamically if it doesn't fit.
            size_t size = 1024;
            char stackbuf[1024];
            std::vector<char> dynamicbuf;
            char* buf = &stackbuf[0];

            while (true) {
                // Try to vsnprintf into our buffer.
                int needed = vsnprintf(buf, size, fmt, args);
                // NB. C99 (which modern Linux and OS X follow) says vsnprintf
                // failure returns the length it would have needed. But older
                // glibc and current Windows return -1 for failure, i.e., not
                // telling us how much was needed.

                if (needed <= (int)size && needed >= 0) {
                    result = std::string(buf, (size_t)needed);    // It fits fine so we're done.
                    return;
                }

                // vsnprintf reported that it wanted to write more characters
                // than we allotted.  So try again using a dynamic buffer.  This
                // doesn't happen very often if we chose our initial size well.
                size = (needed > 0) ? (needed + 1) : (size * 2);
                dynamicbuf.resize(size);
                buf = &dynamicbuf[0];
            }
        }


        void format(std::string& result, const char* fmt, ...)
        {
            va_list args;
            va_start(args, fmt);
            vformat(result, fmt, args);
            va_end(args);
        }


        //-------------------------------------------------------------------------------------------------------
        // String Conversion
        //-------------------------------------------------------------------------------------------------------

        //@param value Value to convert
        //@param text	String up to length char
        //@param maxLen Maximal length of the string
        void DoubleToString(double value, char* text, uint16_t maxLen)
        {
            uint16_t c = 0, neg = 0;
            char string[32];
            char* s;
            double v, integ, i10, mantissa, m10, ten = 10.;

            v = (double)value;
            if (v < 0)
            {
                neg = 1;
                value = -value;
                v = -v;
                c++;
                if (v > 9999999.)
                {
                    strncpy(string, "Huge!", 31);
                    return;
                }
            }
            else if (v > 99999999.)
            {
                strncpy(string, "Huge!", 31);
                return;
            }

            s = string + 31;
            *s-- = 0;
            *s-- = '.';
            c++;

            integ = floor(v);
            i10 = fmod(integ, ten);
            *s-- = (char)((uint16_t)i10 + '0');
            integ /= ten;
            c++;
            while (integ >= 1. && c < 8)
            {
                i10 = fmod(integ, ten);
                *s-- = (char)((uint16_t)i10 + '0');
                integ /= ten;
                c++;
            }
            if (neg)
                *s-- = '-';
            strncpy(text, s + 1, maxLen);
            if (c >= 8)
                return;

            s = string + 31;
            *s-- = 0;
            mantissa = fmod(v, 1.);
            mantissa *= pow(ten, (double)(8 - c));
            while (c < 8)
            {
                if (mantissa <= 0)
                    *s-- = '0';
                else
                {
                    m10 = fmod(mantissa, ten);
                    *s-- = (char)((uint16_t)m10 + '0');
                    mantissa /= 10.;
                }
                c++;
            }
            strncat(text, s + 1, maxLen);
        }


        void IntToString(int32_t value, char* text, uint16_t maxLen)
        {
            if (value >= 100000000) {
                strncpy(text, "Huge!", maxLen);
                return;
            }

            if (value < 0) {
                strncpy(text, "-", maxLen);
                value = -value;
            } else {
                strncpy(text, "", maxLen);
            }

            bool state = false;
            for (uint32_t div = 100000000; div >= 1; div /= 10)
            {
                uint32_t digit = value / div;
                value -= digit * div;
                if (state || digit > 0)
                {
                    char temp[2] = { '0' + (char)digit, '\0' };
                    strncat(text, temp, maxLen);
                    state = true;
                }
            }
        }

        
        void DbToString(double value, char* text, uint16_t maxLen)
        {
            if (value <= 0) {
                strncpy(text, "-oo", maxLen);
            }
            else {
                DoubleToString((double)(20. * log10(value)), text, maxLen);
            }
        }


        void HzToString(double samples, char* text, uint16_t maxLen, double sampleRate)
        {
            if (!samples) {
                DoubleToString(0, text, maxLen);
            }
            else {
                DoubleToString(sampleRate / samples, text, maxLen);
            }
        }

        void MsToString(double samples, char* text, uint16_t maxLen, double sampleRate)
        {
            DoubleToString((double)(samples * 1000. / sampleRate), text, maxLen);
        }


        int StringToInt(const std::string& str)
        {
            std::istringstream iss(str);
            int temp;
            iss >> temp;
            return temp;
        }


        int HexToInt(const std::string& hex)
        {
            unsigned int x;
            std::stringstream ss;
            ss << std::hex << hex;
            ss >> x;

            return x;
        }


        void Tokenize(const std::string& input, const std::string& delimiters, std::vector< std::string >& tokens)
        {
            std::string::size_type last_pos = 0;
            std::string::size_type pos = 0;

            while (true)
            {
                pos = input.find_first_of(delimiters, last_pos);
                if (pos == std::string::npos)
                {
                    std::string token = input.substr(last_pos);
                    if (token.empty() == false)
                        tokens.push_back(token);
                    break;
                }
                else
                {
                    tokens.push_back(input.substr(last_pos, pos - last_pos));
                    last_pos = pos + 1;
                }
            }
        }


        //std:string TCHARToString(LPCTSTR t)
        //{
        //    std::string result;

        //    int len = 1 + wcstombs(NULL, t, 0);
        //    if (len == 0) return result;

        //    char* c = new char[len];
        //    if (c == NULL) throw std::bad_alloc();
        //    c[0] = '\0';

        //    wcstombs(c, t, len);
        //    result = c;
        //    delete[] c;
        //    return result;
        //}


    }// namespace common
} // namespace e3