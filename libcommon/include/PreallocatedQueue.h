
//-----------------------------------------------------------------------
// PreallocatedQueue
//
// A queue that allocates memory for a given number of elements
// and uses a pointer to denote the top of the queue.
// When new elements are pushed into the queue and the preallocated 
// memory is full, new memory is allocated automatically.
//
//------------------------------------------------------------------------

#pragma once

#include <vector>


namespace e3 { namespace common {

template<typename T>
class PreallocatedQueue
{
public:
    // Constructor
    // @size        number of elements that will be preallocated
    // @autoResize  number of elements that will be allocated when there are
    //              no more unused elements available. When autoResize is 0,
    //              there will be no automatic resizing.
    PreallocatedQueue(size_t size=0, int autoResize=0) :
        front_(0),
        autoResize_(autoResize)
    {
        resize(size);
    }

    // Returns the number of preallocated elements.
    size_t size() const                     { return vector_.size(); }

    // Return the number of unused elements. This is the number of preallocated elements
    // minus the size of used elements.
    size_t available() const                { return size() - front_; }

    // Returns true when there are no used elements in the queue.
    bool empty() const                      { return front_ == 0; }

    // Sets the number of elements that will be allocated when there are no more unused elements available.
    // When autoResize is 0, there will be no automatic resizing.
    void setAutoResize(size_t autoResize)   { autoResize_ = autoResize; }

    // Resizes the queue.
    // @s   New size. When the new size is bigger than the previous size, new unused elements will be append.
    //      When new size is smaller than the previous size, all elements in the range previous_size - new_size will be removed.
    void resize(size_t s)
    {
        vector_.resize(s);

        s = size();
        front_ = std::min<size_t>(front_, s);
    }

    // Pushes a value on the front of the queue. When there are no more unused elements available and autoResize
    // is greater than zero, new unused elements will be allocated. The number of newly allocated elements is given
    // the autoResize.
    void push(const T value)
    {
        size_t s = size();

        if(front_ >= s && autoResize_ > 0) {
            resize(s + autoResize_);
        }
        if(front_ <= size()) {
            vector_[front_++] = value;
        }
    }


    // Marks the elements on the front of the queue as unused.
    // When there are no currently used elements a std::exception is thrown.
    void pop()
    {
        if(front_ > 0) {
            front_--;
        }
        else throw std::exception("PreallocatedQueue: queue is empty");
    }


    // Returns a reference to the element at the front of the queue.
    // When there are no currently used elements a std::exception is thrown.
    T& front()
    {
        if(front_ > 0) {
            return vector_[front_-1];
        }
        throw std::exception("PreallocatedQueue: queue is empty");
    }

protected:
    size_t front_;
    size_t autoResize_;

    typedef std::vector<T> VectorT;
    VectorT vector_;
};

}} // namespace e3::common