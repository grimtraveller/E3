
//---------------------------------------------------------------------
// e3_Utilities.h
//
// A collection of common tools
//---------------------------------------------------------------------

#pragma once

#include <cstdint>
#include <string>
#include <vector>


namespace e3 {
    namespace common {


        extern int DoubleToInt(double d);


        // Initializes the random generator with current time.
        //
        extern void InitRandom();

        // Generates a random string.
        // @param length    length of generated string
        // @param charset   chars from which the string will be generated
        //
        extern std::string RandomString(int length,
            const std::string& charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890");

        // Formats the given char array into a std::string using printf format style.
        // This operation is immune to buffer overflow problems.
        //
        extern void format(std::string& result, const char* fmt, ...);

        // Formats the given char array into a std::string.
        // args is the argument list for the format parameters.
        // This operation is immune to buffer overflow problems.
        //
        extern void vformat(std::string& result, const char* fmt, va_list args);


        //-------------------------------------------------------------------------------------------------------
        // String Conversion
        //-------------------------------------------------------------------------------------------------------

        //Returns a string representation of a double value
        //@param value Value to convert
        //@param text	String up to length char
        //@param maxLen Maximal length of the string
        extern void DoubleToString(double value, char* text, uint16_t maxLen);

        //Returns a string representation of an integer value
        //@param value Value to convert
        //@param text	String up to length char
        //@param maxLen Maximal length of the string
        extern void IntToString(int32_t value, char* text, uint16_t maxLen);

        // Returns a string representation of a Db value
        //@param value Value to convert
        //@param text	String up to length char
        //@param maxLen Maximal length of the string
        extern void DbToString(double value, char* text, uint16_t maxLen);

        // Returns a string representation of a frequency value in hertz
        //@param samples Number of samples
        //@param text	String up to length char
        //@param maxLen Maximal length of the string
        //@param sampleRate current sample rate
        extern void HzToString(double samples, char* text, uint16_t maxLen, double sampleRate);

        // Returns a string representation of the duration in milliseconds of \e samples frames.
        //@param samples Number of samples
        //@param text	String up to length char
        //@param maxLen Maximal length of the string
        //@param sampleRate current sample rate
        extern void MsToString(double samples, char* text, uint16_t maxLen, double sampleRate);


        //Converts a string to integer
        extern int StringToInt(const std::string& str);

        // Converts a hex string to integer
        extern int HexToInt(const std::string& hex);

        // Breaks a string into an array of tokens
        // @param input string to be tokenized
        // @delimiters a string containing one or multiple delimiters
        // @tokens vector containing the resulting tokens
        extern void Tokenize(const std::string& input, const std::string& delimiters, std::vector<std::string>& tokens);


        // Converts a TCHAR to std::string
        //std:string TCHARToString(LPCTSTR t);

    } // namespace common
} // namespace e3