#include <cat/bit>
#include <cat/match>

#include "../unit_tests.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"

template <auto value>
struct nttp {
   static constexpr auto member = value;
};

static_assert(cat::is_implicitly_constructible<uword, uword>);
static_assert(!cat::is_implicitly_constructible<idx, uword>);
static_assert(cat::is_implicitly_constructible<uword, idx>);

test(arithmetic) {
   // TODO: Test invalid arithmetic operations with `concept`s like
   // `cat::is_add_assignable<uint4, int4>`.

   using namespace cat::literals;
   using namespace cat::integers;

   // Test accessor value categories.
   {
      int4 const constant_int4 = 0;
      static_assert(!cat::is_lvalue_reference<decltype(constant_int4)>);

      static_assert(
         cat::is_lvalue_reference<decltype(constant_int4.wrap().undef())>);
      static_assert(cat::rvalue<decltype(int4{0}.wrap().undef())>);

      static_assert(cat::is_lvalue_reference<decltype(constant_int4.wrap())>);
      static_assert(cat::rvalue<decltype(int4{0}.wrap())>);

      static_assert(cat::is_lvalue_reference<decltype(constant_int4.sat())>);
      static_assert(cat::rvalue<decltype(int4{0}.sat())>);
   }

   {
      idx const constant_idx = 0;

      static_assert(
         cat::is_lvalue_reference<decltype(constant_idx.wrap().undef())>);
      static_assert(cat::rvalue<decltype(idx{0}.wrap().undef())>);

      static_assert(cat::is_lvalue_reference<decltype(constant_idx.wrap())>);
      static_assert(cat::rvalue<decltype(idx{0}.wrap())>);

      static_assert(cat::is_lvalue_reference<decltype(constant_idx.sat())>);
      static_assert(cat::rvalue<decltype(idx{0}.sat())>);
   }

   {
      uintptr<void> const constant_uptr = nullptr;

      static_assert(
         cat::is_lvalue_reference<decltype(constant_uptr.wrap().undef())>);
      static_assert(cat::rvalue<decltype(uintptr<void>{0}.wrap().undef())>);

      static_assert(cat::is_lvalue_reference<decltype(constant_uptr.wrap())>);
      static_assert(cat::rvalue<decltype(uintptr<void>{0}.wrap())>);

      static_assert(cat::is_lvalue_reference<decltype(constant_uptr.sat())>);
      static_assert(cat::rvalue<decltype(uintptr<void>{0}.sat())>);
   }

   // Test relationship to raw types.
   // TODO: Test `is_unsafe_arithmetic` and `to_raw_arithmetic()`.
   static_assert(cat::is_same<cat::raw_arithmetic_type<int>, int>);
   static_assert(cat::is_same<cat::raw_arithmetic_type<int4>, int4::raw_type>);

   // Test numerals' size,
   static_assert(sizeof(int1) == 1);
   static_assert(sizeof(uint1) == 1);
   static_assert(sizeof(int2) == 2);
   static_assert(sizeof(uint2) == 2);
   static_assert(sizeof(int4) == 4);
   static_assert(sizeof(uint4) == 4);
   static_assert(sizeof(int8) == 8);
   static_assert(sizeof(float4) == 4);
   static_assert(sizeof(float8) == 8);

   // Test that numerals are trivial.
   static_assert(cat::is_trivial<int1>);
   static_assert(cat::is_trivial<uint1>);
   static_assert(cat::is_trivial<int2>);
   static_assert(cat::is_trivial<uint2>);
   static_assert(cat::is_trivial<int4>);
   static_assert(cat::is_trivial<uint4>);
   static_assert(cat::is_trivial<int8>);
   static_assert(cat::is_trivial<uint8>);
   static_assert(cat::is_trivial<float4>);
   static_assert(cat::is_trivial<float8>);

   static_assert(cat::is_trivially_relocatable<int1>);
   static_assert(cat::is_trivially_relocatable<uint1>);
   static_assert(cat::is_trivially_relocatable<int2>);
   static_assert(cat::is_trivially_relocatable<uint2>);
   static_assert(cat::is_trivially_relocatable<int4>);
   static_assert(cat::is_trivially_relocatable<uint4>);
   static_assert(cat::is_trivially_relocatable<int8>);
   static_assert(cat::is_trivially_relocatable<uint8>);
   static_assert(cat::is_trivially_relocatable<float4>);
   static_assert(cat::is_trivially_relocatable<float8>);
   static_assert(cat::is_trivially_relocatable<intptr<void>>);
   static_assert(cat::is_trivially_relocatable<intptr<int>>);
   static_assert(cat::is_trivially_relocatable<uintptr<void>>);
   static_assert(cat::is_trivially_relocatable<uintptr<int>>);

   static_assert(cat::is_trivially_copyable<int1>);

   static_assert(cat::is_integral<int4>);
   static_assert(cat::is_integral<uint4>);
   static_assert(cat::is_integral<intptr<void>>);
   static_assert(cat::is_integral<uintptr<void>>);

   static_assert(cat::is_signed<int>);
   static_assert(cat::is_signed<float>);
   static_assert(cat::is_signed<float4>);
   static_assert(cat::is_signed<int4>);
   static_assert(!cat::is_signed<uint4>);

   static_assert(cat::is_unsigned<unsigned>);
   static_assert(!cat::is_unsigned<float>);
   static_assert(!cat::is_unsigned<float4>);
   static_assert(cat::is_unsigned<uint4>);
   static_assert(!cat::is_unsigned<int4>);

   static_assert(cat::is_signed_integral<int>);
   static_assert(!cat::is_signed_integral<unsigned>);
   static_assert(!cat::is_signed_integral<float>);

   static_assert(!cat::is_unsigned_integral<int>);
   static_assert(cat::is_unsigned_integral<unsigned>);
   static_assert(!cat::is_unsigned_integral<float>);

   static_assert(cat::is_arithmetic<char>);
   static_assert(cat::is_arithmetic<signed char>);
   static_assert(cat::is_arithmetic<unsigned char>);
   static_assert(cat::is_arithmetic<short>);
   static_assert(cat::is_arithmetic<unsigned short>);
   static_assert(cat::is_arithmetic<int>);
   static_assert(cat::is_arithmetic<unsigned int>);
   static_assert(cat::is_arithmetic<long long>);
   static_assert(cat::is_arithmetic<unsigned long long>);

   // Test `int_fixed` and `uint_fixed`.
   static_assert(cat::is_same<cat::int_fixed<1>, int1>);
   static_assert(cat::is_same<cat::int_fixed<2>, int2>);
   static_assert(cat::is_same<cat::int_fixed<4>, int4>);
   static_assert(cat::is_same<cat::int_fixed<8>, int8>);

   static_assert(cat::is_same<cat::uint_fixed<1>, uint1>);
   static_assert(cat::is_same<cat::uint_fixed<2>, uint2>);
   static_assert(cat::is_same<cat::uint_fixed<4>, uint4>);
   static_assert(cat::is_same<cat::uint_fixed<8>, uint8>);

   // Test `int4` constructors and assignment.
   int4 test_int4_1 = 1;
   int4 test_int4_2{};
   cat::verify(test_int4_2 == 0);
   test_int4_2 = 1;
   cat::verify(test_int4_2 == 1);

   // Test `uint4` constructors and assignment.
   uint4 test_uint4_1 = 1u;
   uint4 test_uint4_2;
   test_uint4_2 = 1u;

   // Test `arithmetic` operators.
   int4 int4_add = 1 + test_int4_1;
   int4_add = 1_i4 + test_int4_1;
   int4 int4_sub = 1 - test_int4_1;
   int4_sub = 1_i4 - test_int4_1;

   int8 test_int8 = 100ll - test_int4_1;
   cat::verify(test_int8 == 99);
   test_int8 = 100ll + test_int4_1;
   test_int8 = 100ll * test_int4_1;
   test_int8 = 100ll / test_int4_1;

   cat::verify(int4{1} == int4{1});

   uint4 uint4_add = 1u + test_uint4_1;
   uint4_add = 1_u4 + test_uint4_1;
   uint4 uint4_sub = 1u - test_uint4_1;
   uint4_sub = 1_u4 - test_uint4_1;

   uint8 test_uint8 = 100ull - test_uint4_1;
   cat::verify(test_uint8 == 99u);
   test_uint8 = 100ull + test_uint4_1;
   test_uint8 = 100ull * test_uint4_1;
   test_uint8 = 100ull / test_uint4_1;

   cat::verify(uint4{1} == uint4{1u});

   // Greater than.
   cat::verify(int4{1} > int4{0});
   cat::verify(int4{1} >= int4{0});
   cat::verify(int4{1} >= int4{1});

   // Less than.
   cat::verify(int4{0} < int4{1});
   cat::verify(int4{0} <= int4{0});
   cat::verify(int4{0} <= int4{1});

   // Test `arithmetic_ptr` operators on raw numerals.
   [[maybe_unused]]
   intptr<void> intptr_add_1 = 1 + intptr<void>{0};
   intptr<void> intptr_add_2 = intptr<void>{0} + 1;
   cat::verify(intptr_add_2 == 1);
   intptr<void> intptr_add_3 = 1_i4 + intptr<void>{0};
   cat::verify(intptr_add_3 == 1);
   intptr<void> intptr_add_4 = intptr<void>{0} + 1_i4;
   cat::verify(intptr_add_4 == 1);

   // [[maybe_unused]] intptr<void> intptr_sub_1 = 1 - intptr_add_2;
   intptr<void> intptr_sub_2 = intptr_add_2 - 1;
   cat::verify(intptr_sub_2 == 0);
   intptr<void> intptr_sub_3 = 1_i4 - intptr_add_2;
   cat::verify(intptr_sub_3 == 0);
   intptr<void> intptr_sub_4 = intptr_add_2 - 1_i4;
   cat::verify(intptr_sub_4 == 0);

   // Test `arithmetic_ptr` operators on safe `arithmetic`s.
   intptr_add_2 = 1_i4 + intptr_add_2;
   intptr_add_2 = intptr_add_2 + 1_i4;
   intptr_sub_2 = 1_i4 - intptr_add_2;
   intptr_sub_2 = intptr_add_2 - 1_i4;

   // Test `arithmetic_ptr` operators on other `arithmetic_ptr`s.
   cat::verify((intptr<void>{0} + intptr<void>{1}) == 1);
   cat::verify((intptr<void>{0} - intptr<void>{1}) == -1);

   cat::verify(intptr<void>{1} == intptr<void>{1});

   // Greater than.
   cat::verify(intptr<void>{1} > intptr<void>{0});
   cat::verify(intptr<void>{1} >= intptr<void>{0});
   cat::verify(intptr<void>{1} >= intptr<void>{1});

   // Less than.
   cat::verify(intptr<void>{0} < intptr<void>{1});
   cat::verify(intptr<void>{0} <= intptr<void>{0});
   cat::verify(intptr<void>{0} <= intptr<void>{1});

   // TODO: Test integer promotion rules.
   static_assert(cat::is_same<decltype(intptr<void>{} + 1), intptr<void>>);
   static_assert(cat::is_same<decltype(intptr<void>{} - 1), intptr<void>>);

   static_assert(cat::is_convertible<int2, int4>);
   static_assert(cat::is_convertible<uint2, uint4>);
   static_assert(cat::is_convertible<uint2, int4>);

   // Test integer promotion.
   int1 promote_int1 = 1_i1;
   int2 promote_int2 = promote_int1;
   int4 promote_int4 = promote_int2;
   int8 promote_int8 = promote_int2;
   promote_int8 = promote_int4;
   cat::assert(promote_int8 == 1);
   cat::assert(promote_int8.raw == 1);

   uint1 promote_uint1 = 1_u1;
   uint2 promote_uint2 = promote_uint1;
   uint4 promote_uint4 = promote_uint2;
   uint8 promote_uint8 = promote_uint2;
   promote_uint8 = promote_uint4;
   cat::assert(promote_uint8 == 1u);
   cat::assert(promote_uint8.raw == 1u);

   // Promote small unsigned ints to larger signed ints.
   promote_int2 = promote_uint1;
   promote_int4 = promote_uint2;

   promote_int8 = promote_uint1;
   promote_int8 = promote_uint2;
   promote_int8 = promote_uint4;

   // Signed integers cannot be assigned to unsigned integers ever.
   static_assert(cat::is_assignable<int8, uint4>);
   static_assert(!cat::is_assignable<uint4, int2>);
   static_assert(!cat::is_assignable<uint8, int4>);

   // `int4` pointer arithmetic.
   char address;
   int* p_int4 = (reinterpret_cast<int*>(&address)) + 1_i4;
   p_int4 = 1_i4 + (reinterpret_cast<int*>(&address));
   p_int4 += 1_i4;
   p_int4 = p_int4 - 1_i4;
   p_int4 -= 1_i4;

   // TODO: Get `arithmetic_ptr`'s compound assignment operators on raw
   // pointers working again.

   // p_int4 += intptr<void>{1};
   // p_int4 -= intptr<void>{1};
   // p_int4 += uintptr<void>{1u};
   // p_int4 -= uintptr<void>{1u};

   // `idx` pointer arithmetic.
   idx* p_idx = (reinterpret_cast<idx*>(&address)) + 1_i4;
   p_idx = 1_idx + (reinterpret_cast<idx*>(&address));
   p_idx += 1_idx;
   p_idx = p_idx - 1_idx;
   p_idx -= 1_idx;

   // p_idx += intptr<void>{1};
   // p_idx -= intptr<void>{1};
   // p_idx += uintptr<void>{1u};
   // p_idx -= uintptr<void>{1u};

   // Test `intpr` constructors and assignment.
   intptr<void> intptr_1 = nullptr;
   intptr<void> intptr_2 = nullptr;
   intptr_1 = intptr_1 + intptr_2;

   // Test `arithmetic_ptr` conversions.
   uintptr<void> uintptr_1 = static_cast<uintptr<void>>(intptr_1);
   [[maybe_unused]]
   uintptr<void>::raw_type raw_uintptr =
      static_cast<uintptr<void>::raw_type>(uintptr_1);

   [[maybe_unused]]
   intptr<void> intptr_3 = static_cast<intptr<void>>(uintptr_1);

   // Test `arithmetic_ptr` dereferencing operators.
   int4 integer = 0;
   intptr<int4> int_intptr = &integer;
   *int_intptr = 1;
   cat::verify(integer == 1);

   uint4 uinteger = 0u;
   intptr<uint4> uint_intptr = &uinteger;
   *uint_intptr = 1u;
   cat::verify(uinteger == 1u);

   cat::verify(int_intptr->min() == integer.min());
   cat::verify(int_intptr->max() == integer.max());

   cat::verify(uint_intptr->min() == uinteger.min());
   cat::verify(uint_intptr->max() == uinteger.max());

   // Test `intptr` increment and decrement operators.
   uintptr<void> uincptr{0xdeadbeef};
   cat::verify(++uincptr == 0xdeadbef0);
   cat::verify(uincptr++ == 0xdeadbef1);
   cat::verify(--uincptr == 0xdeadbef0);
   cat::verify(uincptr-- == 0xdeadbeef);

   // Test `<=>`.
   int4 int_less = 0;
   int4 int_more = 2;

   [[maybe_unused]]
   bool is_less = (int_less < int_more);
   cat::verify(is_less);
   is_less = ((0 <=> int_more) < 0);  // NOLINT
   cat::verify(is_less);
   is_less = (0 < int_more);
   cat::verify(is_less);
   is_less = (int_less < 2);
   cat::verify(is_less);

   [[maybe_unused]]
   bool is_more = (int_more > int_less);
   is_more = ((0 <=> int_less) == 0);  // NOLINT
   cat::verify(is_more);
   is_more = (0 < int_more);
   cat::verify(is_more);
   is_more = (int_less < 2);
   cat::verify(is_more);

   // Test generated `!=`.
   cat::verify(4_i4 == 4_i4);
   cat::verify(4_i4 != 2_i4);

   cat::verify(4_u4 == 4_u4);
   cat::verify(4_u4 != 2_u4);

   cat::verify(4_f4 == 4_f4);
   cat::verify(4_f4 != 2_f4);

   // Test matching numerals.
   int4 match_int = 1;
   bool matched = false;

   // Match type.
   cat::match(match_int)(  //
      is_a<uint4>().then_do([]() {
         cat::exit(1);
      }),
      is_a<int4>().then_do([&]() {
         matched = true;
      }));
   cat::verify(matched);

   // Match value.
   matched = false;
   cat::match(match_int)(  //
      is_a(0).then_do([]() {
         cat::exit(1);
      }),
      is_a(1).then_do([&]() {
         matched = true;
      }));
   cat::verify(matched);

   // Test unary operators.
   [[maybe_unused]]
   int4 negative_int4 = -1_i4;
   [[maybe_unused]]
   float4 negative_float4 = -1_f4;
   // [[maybe_unused]] int4 positive_int4 = +1_i4;
   // [[maybe_unused]] float4 positive_float4 = +1_f4;
   [[maybe_unused]]
   int4 negated_int4 = ~1_i4;

   // Test using numerals non-type template parameters.
   [[maybe_unused]]
   nttp<1_i1> nttp_int1{};
   [[maybe_unused]]
   nttp<1_u1> nttp_uint1{};
   [[maybe_unused]]
   nttp<1_i2> nttp_int2{};
   [[maybe_unused]]
   nttp<1_u2> nttp_uint2{};
   [[maybe_unused]]
   nttp<1_i4> nttp_int4{};
   [[maybe_unused]]
   nttp<1_u4> nttp_uint4{};
   [[maybe_unused]]
   nttp<1_i8> nttp_int8{};
   [[maybe_unused]]
   nttp<1_u8> nttp_uint8{};
   [[maybe_unused]]
   nttp<1_f4> nttp_float4{};
   [[maybe_unused]]
   nttp<1_f8> nttp_float8{};

   // Test `make_signed()` and `make_unsigned()`.
   static_assert(cat::is_same<decltype(cat::make_unsigned(1)), unsigned int>);
   static_assert(cat::is_same<decltype(cat::make_signed(1u)), int>);
   static_assert(cat::is_same<decltype(cat::make_unsigned(1_i4)), uint4>);
   static_assert(cat::is_same<decltype(cat::make_signed(1_u4)), int4>);

   static_assert(cat::make_sign_from<int4>(1u) == 1_i4);
   static_assert(cat::make_sign_from(2, 1u) == 1_i4);

   static_assert(cat::is_same<decltype(make_unsigned(idx(1u))), idx>);

   // Test unwrapped numerals in `limits`.
   static_assert(cat::limits<int4>::max() ==  // NOLINT
                 cat::limits<int4::raw_type>::max());
   static_assert(cat::limits<uint8>::max() ==  // NOLINT
                 cat::limits<uint8::raw_type>::max());
   static_assert(cat::limits<float4>::max()
                 == cat::limits<float4::raw_type>::max());

   // Test shorthand `*_min` and `*_max` constants.
   static_assert(idx_max == cat::limits<idx>::max());

   // Test unsigned saturating addition.
   static_assert(cat::sat_add(cat::uint1_max - 3u, 1_u1) < cat::uint1_max);
   static_assert(cat::sat_add(cat::uint1_max - 1u, 1_u1) == cat::uint1_max);
   static_assert(cat::sat_add(cat::uint1_max - 1_u1, 1_u1) == cat::uint1_max);
   static_assert(cat::sat_add(cat::uint1_max.raw, 1_u1) == cat::uint1_max);
   static_assert(cat::sat_add(cat::uint1_max, 100_u1) == cat::uint1_max);

   cat::verify(cat::sat_add(cat::uint1_max - 3u, 1_u1) < cat::uint1_max);
   cat::verify(cat::sat_add(cat::uint1_max - 1u, 1_u1) == cat::uint1_max);
   cat::verify(cat::sat_add(cat::uint1_max - 1u, 1u) == cat::uint1_max);
   cat::verify(cat::sat_add(cat::uint1_max.raw, 1_u1) == cat::uint1_max);
   cat::verify(cat::sat_add(cat::uint1_max, 100_u1) == cat::uint1_max);

   static_assert(cat::sat_add(cat::uint2_max - 3u, 2_u2) < cat::uint2_max);
   static_assert(cat::sat_add(cat::uint2_max - 2u, 2_u2) == cat::uint2_max);
   static_assert(cat::sat_add(cat::uint2_max - 1_u2, 2_u2) == cat::uint2_max);
   static_assert(cat::sat_add(cat::uint2_max.raw, 2_u2) == cat::uint2_max);
   static_assert(cat::sat_add(cat::uint2_max, 100_u2) == cat::uint2_max);

   cat::verify(cat::sat_add(cat::uint2_max - 3u, 2_u2) < cat::uint2_max);
   cat::verify(cat::sat_add(cat::uint2_max - 2u, 2_u2) == cat::uint2_max);
   cat::verify(cat::sat_add(cat::uint2_max - 1_u2, 2_u2) == cat::uint2_max);
   cat::verify(cat::sat_add(cat::uint2_max.raw, 2_u2) == cat::uint2_max);
   cat::verify(cat::sat_add(cat::uint2_max, 100_u2) == cat::uint2_max);

   static_assert(cat::sat_add(cat::uint4_max - 3u, 2_u4) < cat::uint4_max);
   static_assert(cat::sat_add(cat::uint4_max - 2u, 2_u4) == cat::uint4_max);
   static_assert(cat::sat_add(cat::uint4_max - 1u, 2u) == cat::uint4_max);
   static_assert(cat::sat_add(cat::uint4_max.raw, 2_u4) == cat::uint4_max);
   static_assert(cat::sat_add(cat::uint4_max, 100u) == cat::uint4_max);

   cat::verify(cat::sat_add(cat::uint4_max - 3u, 2_u4) < cat::uint4_max);
   cat::verify(cat::sat_add(cat::uint4_max - 2u, 2_u4) == cat::uint4_max);
   cat::verify(cat::sat_add(cat::uint4_max - 1u, 2u) == cat::uint4_max);
   cat::verify(cat::sat_add(cat::uint4_max.raw, 2_u4) == cat::uint4_max);
   cat::verify(cat::sat_add(cat::uint4_max, 100u) == cat::uint4_max);

   static_assert(cat::sat_add(cat::uint8_max - 3u, 2_u8) < cat::uint8_max);
   static_assert(cat::sat_add(cat::uint8_max - 2u, 2_u8) == cat::uint8_max);
   static_assert(cat::sat_add(cat::uint8_max - 1u, 2u) == cat::uint8_max);
   static_assert(cat::sat_add(cat::uint8_max.raw, 2_u8) == cat::uint8_max);
   static_assert(cat::sat_add(cat::uint8_max, 100u) == cat::uint8_max);

   cat::verify(cat::sat_add(cat::uint8_max - 3u, 2_u8) < cat::uint8_max);
   cat::verify(cat::sat_add(cat::uint8_max - 2u, 2_u8) == cat::uint8_max);
   cat::verify(cat::sat_add(cat::uint8_max - 1u, 2u) == cat::uint8_max);
   cat::verify(cat::sat_add(cat::uint8_max.raw, 2_u8) == cat::uint8_max);
   cat::verify(cat::sat_add(cat::uint8_max, 100u) == cat::uint8_max);

   // Test signed saturating addition.
   static_assert(cat::sat_add(cat::int1_max - 3_i1, 1_i1) < cat::int1_max);
   static_assert(cat::sat_add(cat::int1_max - 1_i1, 1_i1) == cat::int1_max);
   static_assert(cat::sat_add(cat::int1_max - 1_i1, 1_i1) == cat::int1_max);
   static_assert(cat::sat_add(cat::int1_max.raw, 1_i1) == cat::int1_max);
   static_assert(cat::sat_add(cat::int1_max, 100_i1) == cat::int1_max);

   cat::verify(cat::sat_add(cat::int1_max - 3, 1_i1) < cat::int1_max);
   cat::verify(cat::sat_add(cat::int1_max - 1_i1, 1_i1) == cat::int1_max);
   cat::verify(cat::sat_add(cat::int1_max - 1_i1, 1_i1) == cat::int1_max);
   cat::verify(cat::sat_add(cat::int1_max.raw, 1_i1) == cat::int1_max);
   cat::verify(cat::sat_add(cat::int1_max, 100_i1) == cat::int1_max);

   static_assert(cat::sat_add(cat::int2_max - 3_i2, 2_i2) < cat::int2_max);
   static_assert(cat::sat_add(cat::int2_max - 2_i2, 2_i2) == cat::int2_max);
   static_assert(cat::sat_add(cat::int2_max - 1_i2, 2_i2) == cat::int2_max);
   static_assert(cat::sat_add(cat::int2_max.raw, 2_i2) == cat::int2_max);
   static_assert(cat::sat_add(cat::int2_max, 100_i2) == cat::int2_max);

   cat::verify(cat::sat_add(cat::int2_max - 3_i2, 2_i2) < cat::int2_max);
   cat::verify(cat::sat_add(cat::int2_max - 2_i2, 2_i2) == cat::int2_max);
   cat::verify(cat::sat_add(cat::int2_max - 1_i2, 2_i2) == cat::int2_max);
   cat::verify(cat::sat_add(cat::int2_max.raw, 2_i2) == cat::int2_max);
   cat::verify(cat::sat_add(cat::int2_max, 100_i2) == cat::int2_max);

   static_assert(cat::sat_add(cat::int4_max - 3, 2_i4) < cat::int4_max);
   static_assert(cat::sat_add(cat::int4_max - 2, 2_i4) == cat::int4_max);
   static_assert(cat::sat_add(cat::int4_max - 1, 2) == cat::int4_max);
   static_assert(cat::sat_add(cat::int4_max.raw, 2_i4) == cat::int4_max);
   static_assert(cat::sat_add(cat::int4_max, 100) == cat::int4_max);

   cat::verify(cat::sat_add(cat::int4_max - 3, 2_i4) < cat::int4_max);
   cat::verify(cat::sat_add(cat::int4_max - 2, 2_i4) == cat::int4_max);
   cat::verify(cat::sat_add(cat::int4_max - 1, 2) == cat::int4_max);
   cat::verify(cat::sat_add(cat::int4_max.raw, 2_i4) == cat::int4_max);
   cat::verify(cat::sat_add(cat::int4_max, 100) == cat::int4_max);

   static_assert(cat::sat_add(cat::int8_max - 3, 2_i8) < cat::int8_max);
   static_assert(cat::sat_add(cat::int8_max - 2, 2_i8) == cat::int8_max);
   static_assert(cat::sat_add(cat::int8_max - 1, 2) == cat::int8_max);
   static_assert(cat::sat_add(cat::int8_max.raw, 2_i8) == cat::int8_max);
   static_assert(cat::sat_add(cat::int8_max, 100) == cat::int8_max);

   cat::verify(cat::sat_add(cat::int8_max - 3, 2_i8) < cat::int8_max);
   cat::verify(cat::sat_add(cat::int8_max - 2, 2_i8) == cat::int8_max);
   cat::verify(cat::sat_add(cat::int8_max - 1, 2) == cat::int8_max);
   cat::verify(cat::sat_add(cat::int8_max.raw, 2_i8) == cat::int8_max);
   cat::verify(cat::sat_add(cat::int8_max, 100) == cat::int8_max);

   // Test unsigned saturating subtraction.
   static_assert(cat::sat_sub(cat::uint1_min + 3u, 1_u1) > cat::uint1_min);
   static_assert(cat::sat_sub(cat::uint1_min + 1u, 1_u1) == cat::uint1_min);
   static_assert(cat::sat_sub(cat::uint1_min + 1_u1, 1_u1) == cat::uint1_min);
   static_assert(cat::sat_sub(cat::uint1_min.raw, 1_u1) == cat::uint1_min);
   static_assert(cat::sat_sub(cat::uint1_min, 100_u1) == cat::uint1_min);

   cat::verify(cat::sat_sub(cat::uint1_min + 3u, 1_u1) > cat::uint1_min);
   cat::verify(cat::sat_sub(cat::uint1_min + 1u, 1_u1) == cat::uint1_min);
   cat::verify(cat::sat_sub(cat::uint1_min + 1u, 1u) == cat::uint1_min);
   cat::verify(cat::sat_sub(cat::uint1_min.raw, 1_u1) == cat::uint1_min);
   cat::verify(cat::sat_sub(cat::uint1_min, 100_u1) == cat::uint1_min);

   static_assert(cat::sat_sub(cat::uint2_min + 3u, 2_u2) > cat::uint2_min);
   static_assert(cat::sat_sub(cat::uint2_min + 2u, 2_u2) == cat::uint2_min);
   static_assert(cat::sat_sub(cat::uint2_min + 1_u2, 2_u2) == cat::uint2_min);
   static_assert(cat::sat_sub(cat::uint2_min.raw, 2_u2) == cat::uint2_min);
   static_assert(cat::sat_sub(cat::uint2_min, 100_u2) == cat::uint2_min);

   cat::verify(cat::sat_sub(cat::uint2_min + 3u, 2_u2) > cat::uint2_min);
   cat::verify(cat::sat_sub(cat::uint2_min + 2u, 2_u2) == cat::uint2_min);
   cat::verify(cat::sat_sub(cat::uint2_min + 1_u2, 2_u2) == cat::uint2_min);
   cat::verify(cat::sat_sub(cat::uint2_min.raw, 2_u2) == cat::uint2_min);
   cat::verify(cat::sat_sub(cat::uint2_min, 100_u2) == cat::uint2_min);

   static_assert(cat::sat_sub(cat::uint4_min + 3u, 2_u4) > cat::uint4_min);
   static_assert(cat::sat_sub(cat::uint4_min + 2u, 2_u4) == cat::uint4_min);
   static_assert(cat::sat_sub(cat::uint4_min + 1u, 2u) == cat::uint4_min);
   static_assert(cat::sat_sub(cat::uint4_min.raw, 2_u4) == cat::uint4_min);
   static_assert(cat::sat_sub(cat::uint4_min, 100u) == cat::uint4_min);

   cat::verify(cat::sat_sub(cat::uint4_min + 3u, 2_u4) > cat::uint4_min);
   cat::verify(cat::sat_sub(cat::uint4_min + 2u, 2_u4) == cat::uint4_min);
   cat::verify(cat::sat_sub(cat::uint4_min + 1u, 2u) == cat::uint4_min);
   cat::verify(cat::sat_sub(cat::uint4_min.raw, 2_u4) == cat::uint4_min);
   cat::verify(cat::sat_sub(cat::uint4_min, 100u) == cat::uint4_min);

   static_assert(cat::sat_sub(cat::uint8_min + 3u, 2_u8) > cat::uint8_min);
   static_assert(cat::sat_sub(cat::uint8_min + 2u, 2_u8) == cat::uint8_min);
   static_assert(cat::sat_sub(cat::uint8_min + 1u, 2u) == cat::uint8_min);
   static_assert(cat::sat_sub(cat::uint8_min.raw, 2_u8) == cat::uint8_min);
   static_assert(cat::sat_sub(cat::uint8_min, 100u) == cat::uint8_min);

   cat::verify(cat::sat_sub(cat::uint8_min + 3u, 2_u8) > cat::uint8_min);
   cat::verify(cat::sat_sub(cat::uint8_min + 2u, 2_u8) == cat::uint8_min);
   cat::verify(cat::sat_sub(cat::uint8_min + 1u, 2u) == cat::uint8_min);
   cat::verify(cat::sat_sub(cat::uint8_min.raw, 2_u8) == cat::uint8_min);
   cat::verify(cat::sat_sub(cat::uint8_min, 100u) == cat::uint8_min);

   // Test signed saturating addition.
   static_assert(cat::sat_sub(cat::int1_min + 3_i1, 1_i1) > cat::int1_min);
   static_assert(cat::sat_sub(cat::int1_min + 1_i1, 1_i1) == cat::int1_min);
   static_assert(cat::sat_sub(cat::int1_min + 1_i1, 1_i1) == cat::int1_min);
   static_assert(cat::sat_sub(cat::int1_min.raw, 1_i1) == cat::int1_min);
   static_assert(cat::sat_sub(cat::int1_min, 100_i1) == cat::int1_min);

   cat::verify(cat::sat_sub(cat::int1_min + 3, 1_i1) > cat::int1_min);
   cat::verify(cat::sat_sub(cat::int1_min + 1_i1, 1_i1) == cat::int1_min);
   cat::verify(cat::sat_sub(cat::int1_min + 1_i1, 1_i1) == cat::int1_min);
   cat::verify(cat::sat_sub(cat::int1_min.raw, 1_i1) == cat::int1_min);
   cat::verify(cat::sat_sub(cat::int1_min, 100_i1) == cat::int1_min);

   static_assert(cat::sat_sub(cat::int2_min + 3_i2, 2_i2) > cat::int2_min);
   static_assert(cat::sat_sub(cat::int2_min + 2_i2, 2_i2) == cat::int2_min);
   static_assert(cat::sat_sub(cat::int2_min + 1_i2, 2_i2) == cat::int2_min);
   static_assert(cat::sat_sub(cat::int2_min.raw, 2_i2) == cat::int2_min);
   static_assert(cat::sat_sub(cat::int2_min, 100_i2) == cat::int2_min);

   cat::verify(cat::sat_sub(cat::int2_min + 3_i2, 2_i2) > cat::int2_min);
   cat::verify(cat::sat_sub(cat::int2_min + 2_i2, 2_i2) == cat::int2_min);
   cat::verify(cat::sat_sub(cat::int2_min + 1_i2, 2_i2) == cat::int2_min);
   cat::verify(cat::sat_sub(cat::int2_min.raw, 2_i2) == cat::int2_min);
   cat::verify(cat::sat_sub(cat::int2_min, 100_i2) == cat::int2_min);

   static_assert(cat::sat_sub(cat::int4_min + 3, 2_i4) > cat::int4_min);
   static_assert(cat::sat_sub(cat::int4_min + 2, 2_i4) == cat::int4_min);
   static_assert(cat::sat_sub(cat::int4_min + 1, 2) == cat::int4_min);
   static_assert(cat::sat_sub(cat::int4_min.raw, 2_i4) == cat::int4_min);
   static_assert(cat::sat_sub(cat::int4_min, 100) == cat::int4_min);

   cat::verify(cat::sat_sub(cat::int4_min + 3, 2_i4) > cat::int4_min);
   cat::verify(cat::sat_sub(cat::int4_min + 2, 2_i4) == cat::int4_min);
   cat::verify(cat::sat_sub(cat::int4_min + 1, 2) == cat::int4_min);
   cat::verify(cat::sat_sub(cat::int4_min.raw, 2_i4) == cat::int4_min);
   cat::verify(cat::sat_sub(cat::int4_min, 100) == cat::int4_min);

   static_assert(cat::sat_sub(cat::int8_min + 3, 2_i8) > cat::int8_min);
   static_assert(cat::sat_sub(cat::int8_min + 2, 2_i8) == cat::int8_min);
   static_assert(cat::sat_sub(cat::int8_min + 1, 2) == cat::int8_min);
   static_assert(cat::sat_sub(cat::int8_min.raw, 2_i8) == cat::int8_min);
   static_assert(cat::sat_sub(cat::int8_min, 100) == cat::int8_min);

   cat::verify(cat::sat_sub(cat::int8_min + 3, 2_i8) > cat::int8_min);
   cat::verify(cat::sat_sub(cat::int8_min + 2, 2_i8) == cat::int8_min);
   cat::verify(cat::sat_sub(cat::int8_min + 1, 2) == cat::int8_min);
   cat::verify(cat::sat_sub(cat::int8_min.raw, 2_i8) == cat::int8_min);
   cat::verify(cat::sat_sub(cat::int8_min, 100) == cat::int8_min);

   // Test wrapping arithmetic operations.
   static_assert(cat::wrap_add(0, 1) == 1);
   static_assert(cat::wrap_add(cat::int4_max - 1, 1) == cat::int4_max);
   static_assert(cat::wrap_add(cat::int4_max, 1) == cat::int4_min);
   static_assert(cat::wrap_add(cat::int4_max, 2) == cat::int4_min + 1);

   static_assert(cat::wrap_add(0u, 1u) == 1u);
   static_assert(cat::wrap_add(cat::uint4_max - 1u, 1u) == cat::uint4_max);
   static_assert(cat::wrap_add(cat::uint4_max, 1u) == cat::uint4_min);
   static_assert(cat::wrap_add(cat::uint4_max, 2u) == cat::uint4_min + 1u);

   static_assert(cat::wrap_mul(0, 1) == 0);
   static_assert(cat::wrap_mul(cat::int4_max, 1) == cat::int4_max);
   static_assert(cat::wrap_mul(cat::int4_max, 2) == -2);

   static_assert(cat::wrap_mul(0u, 1u) == 0u);
   static_assert(cat::wrap_mul(cat::uint4_max, 1u) == cat::uint4_max);
   static_assert(cat::wrap_mul(cat::uint4_max, 2u) == cat::uint4_max - 1u);

   // Test type promotion ordering.
   static_assert(cat::detail::integer_promotion_hierarchy<iword>::order == 8);
   static_assert(cat::detail::integer_promotion_hierarchy<long int>::order
                 == 8);

   // Test overflow semantics.
   int4 safe_int = int4::max();
   cat::verify((safe_int.wrap() + 100) == cat::int4_min + 99);

   safe_int.wrap() += 1;
   cat::verify(safe_int == cat::int4_min);
   safe_int.wrap() += 100;
   cat::verify(safe_int.wrap() == cat::int4_min + 100);
   cat::verify(safe_int.sat() == cat::int4_min + 100);
   cat::verify(safe_int.raw == cat::int4_min + 100);

   // Test saturating overflow with member access syntax.
   safe_int = int4::max();
   cat::verify((safe_int.sat() + 100) == cat::int4_max);

   safe_int.sat() += 1;
   cat::verify(safe_int == cat::int4_max);
   safe_int.sat() += 100;
   cat::verify(safe_int.wrap() == cat::int4_max);
   cat::verify(safe_int.sat() == cat::int4_max);
   cat::verify(safe_int.raw == cat::int4_max);

   // Test overflow strong types.
   cat::wrap_int4 wrap_int4 = cat::int4_max;
   wrap_int4 += 100;
   cat::verify(wrap_int4 == cat::int4_min + 99);
   wrap_int4 = cat::int4_max;
   wrap_int4.sat() += 100;
   cat::verify(wrap_int4 == cat::int4_max);

   cat::sat_int4 saturate_int4 = cat::int4_max;
   saturate_int4 += 100;
   cat::verify(saturate_int4 == cat::int4_max);
   saturate_int4 = cat::int4_max;
   saturate_int4.wrap() += 100;
   cat::verify(saturate_int4 == cat::int4_min + 99);

   // Test integers.
   uword test_uword;
   test_uword = 100u + 0_uz;
   test_uword = 100ull + 0_uz;
   test_uword = 100_u4 + 0_uz;
   test_uword = 100_u8 + 0_uz;

   test_uword = 0_uz + 100u;
   test_uword = 0_uz + 100ull;
   test_uword = 0_uz + 100_u4;
   test_uword = 0_uz + 100_u8;

   test_uword = 100u - 1_uz;
   test_uword = 100ull - 1_uz;
   test_uword = 100_u4 - 1_uz;
   test_uword = 100_u8 - 1_uz;

   test_uword = 200_uz - 100u;
   test_uword = 200_uz - 100ull;
   test_uword = 200_uz - 100_u4;
   test_uword = 200_uz - 100_u8;

   test_uword = 0u;
   test_uword |= 1u;
   cat::verify(test_uword == 1u);

   static_assert(uword(-1) == cat::limits<uword>::max());

   // TODO: Fill this section out.

   // Test floats.
   float4 safe_float = 0.f;
   safe_float = 2.f;
   cat::verify(safe_float == 2.f);
   safe_float.raw = 1.f;
   cat::verify(safe_float == 1.f);

   // Test bit-casts.
   cat::verify(__builtin_bit_cast(unsigned, 2_i4) == 2u);

   // Test `idx`.
   idx idx1;
   idx idx2 = 1;
   idx2 = 1;
   idx idx3 = 1u;
   idx3 = 1u;
   idx1 = idx2;

   idx1 + idx2;
   idx1 + 1;
   1 + idx1;
   1u + idx1;
   1ull + idx1;
   idx1 + 1;
   idx1 + 1u;
   idx1 + 1ull;
   1_uz + idx1;
   1_sz + idx1;
   idx1 + 1_uz;
   idx1 + 1_sz;
   idx1 += 1u;
   idx1++;
   ++idx1;

   idx1 - idx2;
   idx1 - 1;
   10 - idx1;
   10u - idx1;
   10ull - idx1;
   1_uz - idx1;
   1_sz - idx1;
   idx1 - 1_uz;
   idx1 - 1_sz;
   idx1 -= 1;

   idx1* idx2;
   idx1 * 1;
   idx1 * 1ull;
   1 * idx1;
   1u * idx1;
   1ull * idx1;
   1_uz * idx1;
   1_sz * idx1;
   idx1 * 1_uz;
   idx1 * 1_sz;
   idx1 *= 1;

   idx1 / idx2;
   idx1 / 1;
   idx1 / 1ull;
   1 / idx1;
   1u / idx1;
   1ull / idx1;
   1_uz / idx1;
   1_sz / idx1;
   idx _ = idx1 / 1_uz;
   iword _ = idx1 / 1_sz;
   idx1 /= 1;

   idx lesser_idx = 1;
   idx greater_idx = 2;

   cat::verify(lesser_idx == lesser_idx);
   cat::verify(lesser_idx < greater_idx);
   cat::verify(lesser_idx <= greater_idx);
   cat::verify(lesser_idx <= lesser_idx);

   cat::verify(greater_idx == greater_idx);
   cat::verify(greater_idx > lesser_idx);
   cat::verify(greater_idx >= lesser_idx);
   cat::verify(greater_idx >= greater_idx);

   static_assert(!cat::is_implicitly_constructible<idx, uword>);
   static_assert(cat::is_constructible<idx, uword>);
   static_assert(cat::is_implicitly_constructible<uword, idx>);

   static_assert(!cat::is_implicitly_constructible<idx, iword>);
   static_assert(cat::is_constructible<iword, idx>);
   static_assert(cat::is_implicitly_constructible<iword, idx>);

   static_assert(cat::is_unsigned<idx>);
   static_assert(!cat::is_signed<idx>);
   static_assert(cat::is_arithmetic<idx>);
   static_assert(cat::is_safe_arithmetic<idx>);
   static_assert(!cat::is_unsafe_arithmetic<idx>);

   // `idx` is not implicitly convertible to `int`:
   static_assert(!cat::is_convertible<idx, int>);
   static_assert(!cat::is_convertible<idx, int4>);

   // `idx` is implicitly convertible to both word types.
   static_assert(cat::is_safe_arithmetic_conversion<idx, iword>);
   iword index_iword = idx2;
   index_iword = idx2;
   uword index_uword = idx2;
   index_uword = idx2;

   // Unsigned types smaller than `idx` are implicitly convertible.
   static_assert(cat::is_convertible<unsigned char, idx>);
   static_assert(cat::is_convertible<uint1, idx>);
   static_assert(cat::is_convertible<unsigned short, idx>);
   static_assert(cat::is_convertible<uint2, idx>);
   static_assert(cat::is_convertible<unsigned int, idx>);
   static_assert(cat::is_convertible<uint4, idx>);

   auto add_idx_iword = idx2 + 1_sz;
   static_assert(cat::is_same<decltype(add_idx_iword), iword>);
   auto add_iword_idx = 1_sz + idx2;
   static_assert(cat::is_same<decltype(add_iword_idx), iword>);

   // auto add_idx_uword = idx2 + 1_uz;
   // static_assert(cat::is_same<decltype(add_idx_uword), uword>);
   // auto add_uword_idx = 1_uz + idx2;
   // static_assert(cat::is_same<decltype(add_uword_idx), uword>);

   // add_uword_idx += idx2;

   // TODO: `idx` should support += `uword`.
   // TODO: `intptr` should support + `idx`.

   // auto add_int_idx = 1_i4 + idx2;
   // static_assert(cat::is_same<decltype(add_int_idx), idx>);
   // auto add_idx_int = idx2 + 1_i4;
   // static_assert(cat::is_same<decltype(add_idx_int), idx>);
   static_assert(cat::is_same<cat::common_type<idx, iword>, iword>);
   static_assert(cat::is_same<cat::common_type<iword, idx>, iword>);
};

#ifdef __clang__
#pragma GCC diagnostic pop
#endif
