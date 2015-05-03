

//#include <algorithm>
#include <e3_Buffer.h>

using e3::Buffer;

//--------------------------------------------------------
// class BufferTest
//--------------------------------------------------------
//
template <typename T>
class BufferTest : public ::testing::Test
{
public:
    typedef Buffer<T> TBuffer;

    BufferTest() : bufferSize_(100)
    {
        buffer_.resize(bufferSize_);

        for (size_t i = 0; i < bufferSize_; i++) {
            buffer_[i] = (T)i;
        }
    }

    static bool checkValues(const TBuffer& buffer, size_t sizePopulated)
    {
        bool result = true;
        size_t i;
        for (i = 0; i < sizePopulated; i++)
        {
            result &= *(buffer + i) == i;
            result &= *(buffer.getHead() + i) == i;
            result &= buffer[i] == i;
        }
        for (i = sizePopulated; i < buffer.size(); i++)
        {
            result &= buffer[i] == 0;
        }
        return result;
    }

protected:
    TBuffer buffer_;
    size_t bufferSize_;
};

typedef ::testing::Types<char, int, unsigned int, float, double> BufferTestTypes;
TYPED_TEST_CASE(BufferTest, BufferTestTypes);

//----------------------------------------------------------------------------
// Tests
//----------------------------------------------------------------------------

TYPED_TEST(BufferTest, Initialization)
{
    ASSERT_NE(nullptr, buffer_.getHead());
    ASSERT_NE(nullptr, buffer_);
    ASSERT_NE(nullptr, &(buffer_[0]));

    EXPECT_EQ(buffer_.size(), bufferSize_);
    EXPECT_TRUE(checkValues(buffer_, bufferSize_));
}

TYPED_TEST(BufferTest, Copy)
{
    e3::Buffer<TypeParam> buffer2 = buffer_;
    EXPECT_TRUE(checkValues(buffer2, bufferSize_));
}

TYPED_TEST(BufferTest, Clear)
{
    buffer_.clear();
    EXPECT_TRUE(buffer_.empty());
    EXPECT_EQ(buffer_, nullptr);
}

TYPED_TEST(BufferTest, Equality)
{
    TypeParam value = 17;
    TypeParam* ptr = buffer_;

    EXPECT_TRUE(ptr == buffer_);
    EXPECT_EQ(ptr, buffer_);
    EXPECT_EQ(buffer_[17], value);

    EXPECT_TRUE(buffer_.getHead() != &value);
    EXPECT_TRUE(*buffer_ != value);
}


TYPED_TEST(BufferTest, GrowAndShrink)
{
    for (size_t i = bufferSize_; i < bufferSize_ + 1000; i += 5)  // grow often
    {
        ASSERT_NE(nullptr, buffer_.resize(i));
        EXPECT_EQ(buffer_.size(), i);
        EXPECT_TRUE(checkValues(buffer_, bufferSize_));
    }
    for (size_t i = bufferSize_ + 1000; i <= 0; i -= 20)          // shrink often
    {
        ASSERT_NE(nullptr, buffer_.resize(i));
        EXPECT_EQ(buffer_.size(), i);
        EXPECT_TRUE(checkValues(buffer_, std::min<size_t>(bufferSize_, i)));
    }
}

TYPED_TEST(BufferTest, ResizeZero)
{
    buffer_.resize(0);
    buffer_.set(119);
    EXPECT_EQ(buffer_.size(), 0);
    EXPECT_EQ(nullptr, buffer_);
}

TYPED_TEST(BufferTest, ZeroOrOneElement)
{
    Buffer<TypeParam> buffer0;
    EXPECT_EQ(buffer0.size(), 0);
    EXPECT_EQ(buffer0, nullptr);

    Buffer<TypeParam> buffer1(1);
    EXPECT_EQ(*buffer1, 0);
    EXPECT_EQ(buffer1.size(), 1);
    EXPECT_NE(buffer1, nullptr);

    buffer1.remove(0);
    EXPECT_EQ(buffer1.size(), 0);
    EXPECT_EQ(buffer1, nullptr);
}

TYPED_TEST(BufferTest, SetValue)
{
    TypeParam newValue = 17;
    buffer_.set(newValue);
    EXPECT_FALSE(checkValues(buffer_, bufferSize_));

    for (size_t i = 0; i < bufferSize_; i++) {
        EXPECT_EQ(buffer_[i], newValue);
    }
}

TYPED_TEST(BufferTest, RemoveSingle)
{
    EXPECT_EQ(buffer_.size(), 100);
    EXPECT_EQ(buffer_[99], 99);
    EXPECT_NE(buffer_[100], 100);
    
    size_t bufferSize = buffer_.size();
    buffer_.remove(5);
    size_t removed = bufferSize - buffer_.size();
    EXPECT_EQ(removed, 1);
    EXPECT_EQ(buffer_.size(), 99);
    EXPECT_EQ(buffer_[5], 6);
    EXPECT_EQ(buffer_[98], 99);

    buffer_.remove(0);
    EXPECT_EQ(buffer_.size(), 98);
    EXPECT_EQ(buffer_[5], 7);
    EXPECT_EQ(buffer_[97], 99);

    buffer_.remove(97);
    EXPECT_EQ(buffer_.size(), 97);
    EXPECT_EQ(buffer_[96], 98);

    bufferSize = buffer_.size();
    buffer_.remove(0, 0);
    removed = bufferSize - buffer_.size();
    EXPECT_EQ(removed, 0);
    EXPECT_EQ(buffer_.size(), 97);
}

TYPED_TEST(BufferTest, RemoveRange)
{
    size_t bufferSize = buffer_.size();
    buffer_.remove(10, 10);
    size_t removed = bufferSize - buffer_.size();
    EXPECT_EQ(removed, 10);
    EXPECT_EQ(buffer_.size(), 90);
    EXPECT_EQ(buffer_[10], 20);
    EXPECT_EQ(buffer_[89], 99);
}

TYPED_TEST(BufferTest, RemoveOutOfRange)
{
    size_t bufferSize = buffer_.size();
    buffer_.remove(100, 100);
    size_t removed = bufferSize - buffer_.size();
    EXPECT_EQ(removed, 0);
    EXPECT_EQ(buffer_.size(), 100);

    bufferSize = buffer_.size();
    buffer_.remove(90, 100);
    removed = bufferSize - buffer_.size();
    EXPECT_EQ(removed, 10);
    EXPECT_EQ(buffer_.size(), 90);
}

TYPED_TEST(BufferTest, Insert)
{
    Buffer<TypeParam> other(10);
    other.set(-1);

    size_t inserted = buffer_.insert(other, 10, other.size());

    EXPECT_EQ(10, inserted);
    EXPECT_EQ(buffer_.size(), 110);
    EXPECT_EQ(buffer_[10], -1);
    EXPECT_EQ(buffer_[19], -1);
    EXPECT_EQ(buffer_[20], 10);
    EXPECT_EQ(buffer_[109], 99);

    TypeParam value = -2;
    inserted = buffer_.insert(&value, 0, 1);
    EXPECT_EQ(inserted, 1);
    EXPECT_EQ(buffer_.size(), 111);
    EXPECT_EQ(buffer_[0], -2);
    EXPECT_EQ(buffer_[1], 0);
}

TYPED_TEST(BufferTest, InsertOutOfRange)
{
    TypeParam value = 0;
    size_t inserted = buffer_.insert(&value, 100, 1);
    EXPECT_EQ(inserted, 0);
}

