#include <common.hpp>
#include <unsigned.hpp>

#include <array>
#include <cstdlib>
#include <exception>

// NOLINTBEGIN(*-magic-numbers)

namespace {

using namespace big;

/// gUnsignedBig must deduce the minimal bit width for small constants
void
testMinimalBitsForSmallConstants()
{
  static_assert(decltype(gUnsignedBig<0>)::gBits == 0);
  static_assert(decltype(gUnsignedBig<1>)::gBits == 1);
  static_assert(decltype(gUnsignedBig<2>)::gBits == 2);
  static_assert(decltype(gUnsignedBig<3>)::gBits == 2);
  static_assert(decltype(gUnsignedBig<4>)::gBits == 3);
  static_assert(decltype(gUnsignedBig<5>)::gBits == 3);
  static_assert(decltype(gUnsignedBig<6>)::gBits == 3);
  static_assert(decltype(gUnsignedBig<7>)::gBits == 3);
  static_assert(decltype(gUnsignedBig<8>)::gBits == 4);
}

/// gUnsignedBig must deduce the minimal bit width around the 32 bit boundary
void
test32BitBoundary()
{
  static_assert(decltype(gUnsignedBig<0x7FFF'FFFFU>)::gBits == 31);
  static_assert(decltype(gUnsignedBig<0x8000'0000U>)::gBits == 32);
  static_assert(decltype(gUnsignedBig<0x8000'0001U>)::gBits == 32);
  static_assert(decltype(gUnsignedBig<0xFFFF'FFFFULL>)::gBits == 32);
  static_assert(decltype(gUnsignedBig<0x1'0000'0000ULL>)::gBits == 33);
  static_assert(decltype(gUnsignedBig<0x1'0000'0001ULL>)::gBits == 33);
}

/// gUnsignedBig must deduce the minimal bit width around the 64 bit boundary
void
test64BitBoundary()
{
  static_assert(decltype(gUnsignedBig<0x7FFF'FFFF'FFFF'FFFFULL>)::gBits == 63);
  static_assert(decltype(gUnsignedBig<0x8000'0000'0000'0000ULL>)::gBits == 64);
  static_assert(decltype(gUnsignedBig<0x8000'0000'0000'0001ULL>)::gBits == 64);
  static_assert(decltype(gUnsignedBig<0xFFFF'FFFF'FFFF'FFFFULL>)::gBits == 64);
}

/// gUnsignedBig must trim multi element big values to their minimal bit width
void
testMinimalBitsForBigConstants()
{
  constexpr auto allOnes    = UnsignedBig{ std::array{
    0xFFFFFFFFU,
    0xFFFFFFFFU,
    0xFFFFFFFFU,
    0xFFFFFFFFU,
    0xFFFFFFFFU,
    0xFFFFFFFFU,
    0xFFFFFFFFU,
    0xFFFFFFFFU,
    0xFFFFFFFFU,
  } };
  constexpr auto powerOfTwo = UnsignedBig{ std::array{ 0x0U, 0x0U, 0x0U, 0x0U, 0x0U, 0x0U, 0x0U, 0x0U, 0x0U, 0x1U } };

  static_assert(decltype(powerOfTwo)::gBits == 10 * gBitsOf<U32>);
  static_assert(decltype(gUnsignedBig<powerOfTwo>)::gBits == (9 * gBitsOf<U32>)+1);
  static_assert(decltype(gUnsignedBig<powerOfTwo - 1>)::gBits == 9 * gBitsOf<U32>);
  static_assert(decltype(gUnsignedBig<allOnes>)::gBits == 9 * gBitsOf<U32>);
}

} // namespace

int
main(int /* argc */, char** /* argv */)
{
  try {
    testMinimalBitsForSmallConstants();
    test32BitBoundary();
    test64BitBoundary();
    testMinimalBitsForBigConstants();
  } catch (std::exception const&) {
    std::exit(1); // NOLINT(*-mt-unsafe)
  }
}

// NOLINTEND(*-magic-numbers)
