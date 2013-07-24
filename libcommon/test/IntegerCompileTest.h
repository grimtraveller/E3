
#pragma once

#include <limits>

#include <IntegerTypes.h>


// Compile-time assertion
// Use of ctassert<E>, where E is a constant expression,
// will cause a compile-time error unless E evaulates to
// a nonzero integral value.

template <bool t>
struct ctassert {
  enum { N = 1 - 2 * int(!t) };		// 1 if t is true, -1 if t is false.
  static char A[N];
};

template <bool t>
char ctassert<t>::A[N];


// Compile.time tests for integer numbers
//
namespace e3 { namespace integer_test {

	using namespace std;

	const static int8    int8_    = INT8_C(-127);
	const static intl8   intl8_   = INT8_C(-127);
	const static intf8   intf8_   = INT8_C(-127);

	const static uint8   uint8_   = UINT8_C(255);
	const static uintl8  uintl8_  = UINT8_C(255);
	const static uintf8  uintf8_  = UINT8_C(255);

	const static int16   int16_   = INT16_C(-32767);
	const static intl16  intl16_  = INT16_C(-32767);
	const static intf16  intf16_  = INT16_C(-32767);

	const static uint16  uint16_  = UINT16_C(65535);
	const static uintl16 uintl16_ = UINT16_C(65535);
	const static uintf16 uintf16_ = UINT16_C(65535);

	const static int32   int32_   = INT32_C(-2147483647);
	const static intl32  intl32_  = INT32_C(-2147483647);
	const static intf32  intf32_  = INT32_C(-2147483647);

	const static uint32  uint32_  = UINT32_C(4294967295);
	const static uintl32 uintl32_ = UINT32_C(4294967295);
	const static uintf32 uintf32_ = UINT32_C(4294967295);

	const static int64   int64_   = INT64_C(-9223372036854775807);
	const static intl64  intl64_  = INT64_C(-9223372036854775807);
	const static intf64  intf64_  = INT64_C(-9223372036854775807);

	const static uint64  uint64_  = UINT64_C(18446744073709551615);
	const static uintl64 uintl64_ = UINT64_C(18446744073709551615);
	const static uintf64 uintf64_ = UINT64_C(18446744073709551615);

	const static boost::intmax_t intmax_    = INTMAX_C(-9223372036854775807);
	const static boost::uintmax_t uintmax_  = UINTMAX_C(18446744073709551615);

	ctassert< int8(0)   == INT8_C(0) >   t1;  
	ctassert< uint8(0)  == UINT8_C(0) >  t2;  
	ctassert< int16(0)  == INT16_C(0) >  t3;  
	ctassert< uint16(0) == UINT16_C(0) > t4;
	ctassert< int32(0)  == INT32_C(0) >  t5;  
	ctassert< uint32(0) == UINT32_C(0) > t6;
	ctassert< int64(0)  == INT64_C(0) >  t7;  
	ctassert< uint64(0) == UINT64_C(0) > t8;

	ctassert< int8_    == -127 > t10;
	ctassert< intl8_   == -127 > t11;
	ctassert< intf8_   == -127 > t12;
	ctassert< uint8_   == 255u > t13;
	ctassert< uintl8_  == 255u > t14;
	ctassert< uintf8_  == 255u > t15;
	ctassert< int16_   == -32767 > t16;
	ctassert< intl16_  == -32767 > t17;
	ctassert< intf16_  == -32767 > t18;
	ctassert< uint16_  == 65535u > t19;
	ctassert< uintl16_ == 65535u > t20;
	ctassert< uintf16_ == 65535u > t21;
	ctassert< int32_   == -2147483647 > t22;
	ctassert< intl32_  == -2147483647 > t23;
	ctassert< intf32_  == -2147483647 > t24;
	ctassert< uint32_  == 4294967295u > t25;
	ctassert< uintl32_ == 4294967295u > t26;
	ctassert< uintf32_ == 4294967295u > t27;
	ctassert< int64_   == INT64_C(-9223372036854775807) > t28;
	ctassert< intl64_  == INT64_C(-9223372036854775807) > t29;
	ctassert< intf64_  == INT64_C(-9223372036854775807) > t30;
	ctassert< uint64_  == UINT64_C(18446744073709551615) > t31;
	ctassert< uintl64_ == UINT64_C(18446744073709551615) > t32;
	ctassert< uintf64_ == UINT64_C(18446744073709551615) > t33;
	ctassert< intmax_  == INT64_C(-9223372036854775807) > t34;
	ctassert< uintmax_ == UINT64_C(18446744073709551615) > t35;

	ctassert< numeric_limits<int8>::is_integer > t40;
	ctassert< numeric_limits<uint8>::is_integer > t41;
	ctassert< numeric_limits<int16>::is_integer > t42;
	ctassert< numeric_limits<uint16>::is_integer > t43;
	ctassert< numeric_limits<int32>::is_integer > t44;
	ctassert< numeric_limits<uint32>::is_integer > t45;

	ctassert< numeric_limits<int8>::is_signed> t46;
	ctassert< numeric_limits<int16>::is_signed > t47;
	ctassert< numeric_limits<int32>::is_signed > t48;

	ctassert< !numeric_limits<uint8>::is_signed > t49;
	ctassert< !numeric_limits<uint16>::is_signed > t50;
	ctassert< !numeric_limits<uint32>::is_signed > t51;

	ctassert< numeric_limits<int8>::radix == 2 > t52;
	ctassert< numeric_limits<uint8>::radix == 2 > t53;
	ctassert< numeric_limits<int16>::radix == 2 > t54;
	ctassert< numeric_limits<uint16>::radix == 2 > t55;
	ctassert< numeric_limits<int32>::radix == 2 > t56;
	ctassert< numeric_limits<uint32>::radix == 2 > t57;

	ctassert< numeric_limits<int8>::digits == 7 > t58;
	ctassert< numeric_limits<uint8>::digits == 8 > t59;
	ctassert< numeric_limits<int16>::digits == 15 > t60;
	ctassert< numeric_limits<uint16>::digits == 16 > t61;
	ctassert< numeric_limits<int32>::digits == 31 > t62;
	ctassert< numeric_limits<uint32>::digits == 32 > t63;

}} // namespace e3::integer_test

