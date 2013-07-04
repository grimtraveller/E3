
//------------------------------------------------------------
// A generic buffer class
//
// A Buffer can be resized at any time.
// Only minimal error checking is applied.
//------------------------------------------------------------

#pragma once

#include <LibCommon.h>


template < class T >
class Buffer
{
protected:
    T* data_;
	size_t size_;

public:

	//---------------------------------------------------
	// Constructor
	// Creates a buffer and allocates memory, which is left uninitialized.
	//
	// size_t size	the size of memory to be allocated
	//----------------------------------------------------
	Buffer( size_t size = 0 )
	: data_( NULL ), 
	  size_( size )
	{
		data_ = allocate( size_ );
	}

	//---------------------------------------------------
	// Constructor
	// Creates a buffer, allocates memory and initializes the memory with
	// the specified value.
	//
	// const T& value	the value used to initialize the memory
	// size_t size		the size of memory to be allocated
	//----------------------------------------------------
	Buffer( const T& value, size_t size ) :	
        data_( NULL ), 
		size_( size )
	{
		data_ = allocate( size_ );

		if( data_ != NULL ) {
            memset(data_, value, size_);
		}
	}

    //-------------------------------------------------------
    // Copy Constructor
    //-------------------------------------------------------
    Buffer(const Buffer& source) :
        data_(NULL),
        size_(0)
    {
        copy(source);
    }

	//-------------------------------------------------------
	// Destructor
	// Frees all allocated memory.
	//-------------------------------------------------------
	virtual ~Buffer() {	clear(); }


    //-------------------------------------------------------
    // Copies the data from the given buffer
    //-------------------------------------------------------
    void copy(const Buffer& source)
    {
        clear();
        size_ = source.size_;
        data_ = allocate(size_);

        if(data_ != NULL) {
            memcpy(data_, source.data_, size_ * sizeof(T));
        }
    }

    //--------------------------------------------------------
    // Assignment operator
    //--------------------------------------------------------
    virtual Buffer& Buffer::operator= (const Buffer& source)
    {
        copy(source);
        return *this;     
    }

    //------------------------------------------------------
    // Return the pointer to the memory
    //------------------------------------------------------
    T* getHead() const { return data_; }

	//-------------------------------------------------------
	// Returns the size of the currently allocated memory
	// ------------------------------------------------------
    size_t getSize() const { return size_; }

	//------------------------------------------------------
	// Returns true, if the size of the allocated memory is zero.
	//------------------------------------------------------
	bool empty() const	{ return size_ == 0; }


	//------------------------------------------------------
    // Resizes the buffer, preserving existing memory
    //------------------------------------------------------
    virtual T* resize( size_t size )
	{
		if( data_ != NULL )		// preserve data
		{
			T* oldData = data_;
			data_      = allocate( size );

			memcpy( data_, oldData, std::min<size_t>( size, size_ ) * sizeof( T ));
			free( oldData );
		}
		else {
			data_ = allocate( size );
		}
        size_ = (data_) ? size : 0;

		return data_;
	}

	//------------------------------------------------------------------------
    // Resizes the buffer and initializes the memory with the specified value
    //------------------------------------------------------------------------
	T* resize( size_t size, T value )
	{
		resize( size );
		set( value );
		
		return data_;
	}


	void set( T value )
	{
		for( size_t i=0; i<size_; i++ ) {
			data_[i] = value;
		}
	}

    T* removeAt( size_t idx )
    {
		if( data_ != NULL )		// preserve data
		{
            ASSERT( idx >= 0 && idx < size_ );
            T* oldData = data_;
			data_      = allocate( size_ - 1 );

            if( data_ ) {
                memcpy( data_,     oldData,       idx * sizeof( T ));
                memcpy( data_+idx, oldData+idx+1, (size_-(idx+1)) * sizeof( T ));
            }

            --size_;
			free( pOldData );
            if( size_ == 0 ) {
                ASSERT( data_ == NULL );
            }
		}
        return data_;
    }


    T* removeRange( size_t idx, size_t length )
    {
		ASSERT( length <= size_-idx-1 );
        length = min( size_-idx-1, length );

        if( data_ != NULL )		// preserve data
		{
            ASSERT( idx >= 0 && idx < size_ );
            T* oldData = data_;
			data_      = allocate( size_ - length );

            if( data_ ) {
                memcpy( data_,     oldData,            idx * sizeof( T ));
                memcpy( data_+idx, oldData+idx+length, (size_-(idx+length)) * sizeof( T ));
            }

            size_ -= length;
			free( pOldData );
            if( size_ == 0 ) {
                ASSERT( data_ == NULL );
            }
		}
        return data_;
    }


    virtual void clear()
	{
		if( data_ ) {
			free( data_ );
			data_ = NULL;
            size_ = 0;
		}
	}

protected:
	T* allocate( size_t size )
	{
		T* ptr = NULL;

		if( size > 0 ) 
		{
			ptr = (T*)calloc( size, sizeof( T ));
			ASSERT( ptr != NULL );
		}
		return ptr;
	}
};

