
//------------------------------------------------------------------
// class RecordBuffer
//
// A templated circular buffer using std::vector as storage container
//------------------------------------------------------------------

#pragma once

#include <vector>
#include <stdexcept>

namespace e3 { namespace log {

enum OverflowPolicy 
{
    ON_OVERFLOW_USE_OLDEST,
    ON_OVERFLOW_USE_NEWEST,
    ON_OVERFLOW_RESIZE,
    ON_OVERFLOW_THROW
};


class OverflowException : public std::runtime_error {
public:
    OverflowException() : std::runtime_error("") {};
};
class UnderflowException : public std::runtime_error {
public:
    UnderflowException() : std::runtime_error("") {};
};



template<typename T>
class RecordBuffer
{
protected:
    typedef std::vector<T> VectorT;

public:
    RecordBuffer(size_t size=0) : 
      read_(0),
      write_(0),
      used_(0),
      overflowPolicy_(ON_OVERFLOW_USE_NEWEST)
    {
        resize(size);
    }


    void resize(size_t size)
    {
        vector_.resize(size);
        size_ = vector_.size();
    }


    void clear()
    {
        VectorT().swap(vector_);      // clear records_, explicitly deallocating memory
        size_  = vector_.size();
        read_  = 0;
        write_ = 0;
        used_  = 0;
    }


    size_t size() const         { return size_; }
    size_t available() const    { return size_ - used_; }
    bool empty() const          { return used_ == 0; }
    bool full() const           { return (read_ == write_ || write_ >= size_) && used_ > 0; }
    
    void setOverflowPolicy(OverflowPolicy p)  { overflowPolicy_ = p; }

    
    void push(const T& value)
    {
        if(full() == false)
        {
            vector_[write_] = value;
            write_ = (write_ + 1) % size_;
            used_  = std::min(++used_, size_);
        }
        else
        {
            switch(overflowPolicy_)
            {
            case ON_OVERFLOW_USE_OLDEST:        // dont push new values
                return;

            case ON_OVERFLOW_USE_NEWEST:        // push new values, overwriting old values
                vector_[write_] = value;
                write_ = (write_ + 1) % size_;
                break;

            case ON_OVERFLOW_RESIZE:            // resize and push new values
                resize(size_t(size_ * 1.5));
                vector_[used_] = value;
                used_++;
                write_ = used_;
                break;

            case ON_OVERFLOW_THROW:             // throw OverflowException
                throw OverflowException();
            
            default: return;
            }
        }
    }


    const T& pop()
    {
        if(empty())
            throw UnderflowException();

        const T& result = vector_[read_];

        read_ = (read_ + 1) % size_;
        used_--;

        return result;
    }

    
    // Returns the element at the current write position.
    // The read and write pointers are left unchanged.
    T& getElementAtWritePosition()
    {
        return vector_[write_];
    }


protected:
    size_t size_;       // The allocated size
    size_t write_;      // Array index denoting the write position
    size_t read_;       // Array index denoting the read position
    size_t used_;       // Number of elements in use.

    OverflowPolicy overflowPolicy_;
    VectorT vector_;
};

}} // namespace e3::log