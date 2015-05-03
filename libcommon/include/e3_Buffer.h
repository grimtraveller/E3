
//------------------------------------------------------------
// A generic buffer class
//
// A Buffer can be resized at any time.
// Only minimal error checking is applied.
//------------------------------------------------------------

// TODO: remove virtual methods

#pragma once

#include <e3_Exception.h>


namespace e3 {

    template < class T >
    class Buffer
    {
    protected:
        T* data_;
        size_t size_;

    public:

        //---------------------------------------------------
        // Constructor
        // Creates a buffer and allocates memory.
        //
        // size_t size	the size of memory to be allocated
        //----------------------------------------------------
        Buffer(size_t size = 0)
            : data_(nullptr),
            size_(size)
        {
            data_ = allocate(size_);
        }

        //---------------------------------------------------
        // Constructor
        // Creates a buffer, allocates memory and initializes the memory with
        // the specified value.
        //
        // const T& value	the value used to initialize the memory
        // size_t size		the size of memory to be allocated
        //----------------------------------------------------
        Buffer(const T& value, size_t size) :
            data_(nullptr),
            size_(size)
        {
            data_ = allocate(size_);

            if (data_ != nullptr) {
                memset(data_, value, size_);
            }
        }

        //-------------------------------------------------------
        // Copy Constructor
        //-------------------------------------------------------
        Buffer(const Buffer& source) :
            data_(nullptr),
            size_(0)
        {
            copy(source);
        }

        //-------------------------------------------------------
        // Destructor
        // Frees all allocated memory.
        //-------------------------------------------------------
        virtual ~Buffer() { clear(); }


        //-------------------------------------------------------
        // Copies the data from the given buffer
        //-------------------------------------------------------
        void copy(const Buffer& source)
        {
            clear();
            size_ = source.size_;
            data_ = allocate(size_);

            if (data_ != nullptr) {
                memcpy(data_, source.data_, size_ * sizeof(T));
            }
        }

        //------------------------------------------------------
        // Returns a pointer to the allocated memory
        //
        T* getHead() const { return data_; }

        //-------------------------------------------------------
        // Returns the size of the currently allocated memory
        //
        size_t size() const { return size_; }

        //------------------------------------------------------
        // Returns true, if the size of the allocated memory is zero.
        //
        bool empty() const	{ return size_ == 0; }

        //--------------------------------------------------------
        // Assignment operator
        //
        virtual Buffer& operator= (const Buffer& source)
        {
            copy(source);
            return *this;
        }

        //----------------------------------------------------------------------------------------------
        // Returns a raw pointer to the allocated data.
        //    This may be a null pointer if the data hasn't yet been allocated, or if it has been
        //    freed by calling the free() method.
        //
        inline operator T*() const { return data_; }

        //----------------------------------------------------------------------------------------------
        // Returns a reference to one of the data elements.
        //    No bounds-checking.
        //
        //inline T& operator[] (size_t index) const { return data_[index]; }

        //----------------------------------------------------------------------------------------------
        // Returns a pointer to a data element at an offset from the start of the memory block.
        //    This is the same as doing pointer arithmetic on the raw pointer itself.
        //
        inline T* operator+ (size_t index) const { return data_ + index; }


        //------------------------------------------------------------------------------------------
        // Compares the pointer with another pointer.
        // This can be handy for checking whether this is a null pointer.
        //------------------------------------------------------------------------------------------
        inline bool operator== (const T* const other) const { return other == data_; }

        //------------------------------------------------------------------------------------------            
        // Compares the pointer with another pointer.
        // This can be handy for checking whether this is a null pointer.
        //------------------------------------------------------------------------------------------
        inline bool operator!= (const T* const other) const { return other != data_; }


        //------------------------------------------------------
        // Resizes the buffer, preserving existing data
        //------------------------------------------------------
        virtual T* resize(size_t size)
        {
            if (data_ != nullptr)		// preserve data
            {
                T* oldData = data_;
                data_ = allocate(size);

                memcpy(data_, oldData, std::min<size_t>(size, size_) * sizeof(T));
                free(oldData);
            }
            else {
                data_ = allocate(size);
            }
            size_ = (data_) ? size : 0;

            return data_;
        }

        //------------------------------------------------------------------------
        // Resizes the buffer and initializes the memory with the specified value
        //------------------------------------------------------------------------
        T* resize(size_t size, T value)
        {
            resize(size);
            set(value);

            return data_;
        }


        void set(T value)
        {
            for (size_t i = 0; i < size_; i++) {
                data_[i] = value;
            }
        }


        //----------------------------------------------------------------------
        // Removes and deallocates the range of data from first to pos+length.
        // The remaining data is preserved.
        // If pos+length exceeds size, length is truncated.
        // @return pointer to the data
        //----------------------------------------------------------------------
        T* remove(size_t pos, size_t length=1)
        {
            // Check bounds and truncate length if needed
            length = std::min<size_t>(size_ - pos, length);

            if (length == 0 || pos >= size_) return 0;

            if (data_ != nullptr)		// remove and preserve data
            {
                T* oldData = data_;
                data_ = allocate(size_ - length);

                if (data_) {
                    memcpy(data_, oldData, pos * sizeof(T));
                    memcpy(data_ + pos, oldData + pos + length, (size_ - (pos + length)) * sizeof(T));
                    size_ -= length;
                }
                else {
                    size_ = 0;
                }
                
                free(oldData);
                if (size_ == 0) {
                    ASSERT(data_ == nullptr);
                }
            }
            return data_;
        }

        // Inserts a memory block into the buffer. Existing data is preserved.
        // The memory block is copied and no ownership is taken.
        // If pos exceeds buffer size, nothing is inserted.
        // @return length of the inserted memory block.
        //
        size_t insert(T* other, size_t pos, size_t length)
        {
            if (pos >= size_) return 0;
            
            if (data_ != nullptr)		// insert and preserve data
            {
                T* oldData = data_;
                data_ = allocate(size_ + length);

                if (data_) {
                    size_t sizeofT = sizeof(T);
                    memcpy(data_, oldData, pos * sizeofT);
                    memcpy(data_ + pos, other, length * sizeofT);
                    memcpy(data_ + pos + length, oldData + pos, (size_ - pos) * sizeofT);
                    size_ += length;
                }
                else {
                    size_ = 0;
                }

                free(oldData);
                if (size_ == 0) {
                    ASSERT(data_ == nullptr);
                }
            }
            return length;
        }


        virtual void clear()
        {
            if (data_) {
                ::free(data_);
                data_ = nullptr;
                size_ = 0;
            }
        }

    protected:
        T* allocate(size_t size)
        {
            T* ptr = nullptr;

            if (size > 0)
            {
                ptr = static_cast<T*>(calloc(size, sizeof(T)));
                ASSERT(ptr != nullptr);
            }
            return ptr;
        }
    };

} // namespace e3