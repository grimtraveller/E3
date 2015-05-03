
#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>
#include <type_traits>



namespace e3 {

    //----------------------------------------------------------------
    // Computes Lambert W function
    // @param z:  [-1/e, inf)
    // @return LambertW(z) satisfies W(z)*exp(W(z))=z
    //----------------------------------------------------------------
    //
    extern double LambertW(const double z);


    //-----------------------------------------------------------------
    // Clips the number n between lower and upper
    //-----------------------------------------------------------------
    //
    template <typename T>
    T clip(const T& n, const T& lower, const T& upper) 
    {
        return std::max(lower, std::min(n, upper));
    }


    template <typename T>
    class BoundedRange
    {
        static_assert(std::is_arithmetic<T>::value, "class BoundedRange can only be instantiated with numeric types");

    public:
        BoundedRange(const T& min = 0, const T& max = 1) { validate(min, max); }

        T bound(const T& value) const { return clip<T>(value, min_, max_); }

        void setMin(T min)            { validate(min, max_); }
        void setMax(T max)            { validate(min_, max); }

        T getMin() const              { return min_; }
        T getMax() const              { return max_; }
        T getRange() const            { return max_ - min_; }


    protected:
        void validate(T min, T max)
        {
            if (min > max)
                std::swap(min, max);
            min_ = min;
            max_ = max;
        }

        T min_;
        T max_;
    };


    //----------------------------------------------------------------
    // class BoundedValue
    // Stores a numerical value and keeps it within a range.
    //----------------------------------------------------------------

    template <typename T>
    class BoundedValue : public BoundedRange<T>
    {
        static_assert(std::is_arithmetic<T>::value, "class BoundedValue can only be instantiated with numeric types");

    public:
        BoundedValue(T value=0, T min=0, T max=1) { validate(value, min, max); }
                                                 
        void setValue(T value)                    { validate(value, min_, max_); }
        T getValue() const                        { return value_; }
                                              
        BoundedValue& operator= (const T& other)  { validate(other, min_, max_); return *this; }
        operator T() const                        { return value_; }

    private:
        void validate(T value, T min, T max)
        {
            BoundedRange<T>::validate(min, max);
            value_ = clip<T>(value, min_, max_);
        }

        T value_;
    };


    // Converts pitch number to frequency
    //extern double PitchToFreq(double pitch);
    inline double PitchToFreq(double pitch)
    {
        return (440.0 * pow(2.0, (pitch - 69.0) / 12.0));
    }

    // Converts frequency to pitch number
    //extern double FreqToPitch(double freq);
    inline double FreqToPitch(double freq)
    {
        return log10(freq / 8.17742) / log10(1.0594630943593);
    }

    template< typename T >
    bool IsInfPos(T value)
    {
        return std::numeric_limits<T>::has_infinity && value == std::numeric_limits<T>::infinity();
    }

    template< typename T >
    bool IsNAN(T value)
    {
        return value != value;
    }

    // Byte-swaps a 16-bit data type.
    extern void Swap16(unsigned char* ptr);

    // Byte-swaps a 32-bit data type.
    extern void Swap32(unsigned char* ptr);

    // Byte-swaps a 64-bit data type.
    extern void Swap64(unsigned char* ptr);

    // Combines the first 7 bits of value1 with the first 7 bits of value2,
    // as used by pitchbend messages.
    extern uint16_t Combine14Bits(uint16_t value1, uint16_t value2);




} // namespace e3

