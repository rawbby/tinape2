#include <common.hpp>

#include <cstdlib>
#include <exception>

// NOLINTBEGIN(*-magic-numbers)

namespace {

/// gHiMask must select the requested number of high bits
void
testHiMaskSelectsHighBits()
{
  static_assert(gHiMask<U8, 1> == 0b10000000);
  static_assert(gHiMask<U8, 2> == 0b11000000);
  static_assert(gHiMask<U8, 3> == 0b11100000);
  static_assert(gHiMask<U8, 4> == 0b11110000);
  static_assert(gHiMask<U8, 5> == 0b11111000);
  static_assert(gHiMask<U8, 6> == 0b11111100);
  static_assert(gHiMask<U8, 7> == 0b11111110);

  static_assert(gHiMask<U16, 1> == 0b1000000000000000);
  static_assert(gHiMask<U16, 7> == 0b1111111000000000);
  static_assert(gHiMask<U16, 15> == 0b1111111111111110);

  static_assert(gHiMask<U32, 1> == 0b10000000000000000000000000000000);
  static_assert(gHiMask<U32, 7> == 0b11111110000000000000000000000000);
  static_assert(gHiMask<U32, 31> == 0b11111111111111111111111111111110);

  static_assert(gHiMask<U64, 1> == 0b1000000000000000000000000000000000000000000000000000000000000000);
  static_assert(gHiMask<U64, 7> == 0b1111111000000000000000000000000000000000000000000000000000000000);
  static_assert(gHiMask<U64, 63> == 0b1111111111111111111111111111111111111111111111111111111111111110);
}

/// zero bits requested must produce an empty mask
void
testHiMaskZeroBitsIsEmpty()
{
  static_assert(gHiMask<U8, 0> == 0);
  static_assert(gHiMask<U16, 0> == 0);
  static_assert(gHiMask<U32, 0> == 0);
  static_assert(gHiMask<U64, 0> == 0);
}

/// requesting all bits must produce a full mask
void
testHiMaskAllBitsIsFull()
{
  static_assert(gHiMask<U8, 8> == 0b11111111);
  static_assert(gHiMask<U16, 16> == 0b1111111111111111);
  static_assert(gHiMask<U32, 32> == 0b11111111111111111111111111111111);
  static_assert(gHiMask<U64, 64> == 0b1111111111111111111111111111111111111111111111111111111111111111);
}

/// requesting more bits than the type holds must saturate to a full mask
void
testHiMaskSaturatesBeyondTypeWidth()
{
  static_assert(gHiMask<U8, 9> == 0b11111111);
  static_assert(gHiMask<U8, 10> == 0b11111111);
  static_assert(gHiMask<U16, 17> == 0b1111111111111111);
  static_assert(gHiMask<U16, 18> == 0b1111111111111111);
  static_assert(gHiMask<U32, 33> == 0b11111111111111111111111111111111);
  static_assert(gHiMask<U32, 34> == 0b11111111111111111111111111111111);
  static_assert(gHiMask<U64, 65> == 0b1111111111111111111111111111111111111111111111111111111111111111);
  static_assert(gHiMask<U64, 66> == 0b1111111111111111111111111111111111111111111111111111111111111111);
}

} // namespace

int
main(int /* argc */, char** /* argv */)
{
  try {
    testHiMaskSelectsHighBits();
    testHiMaskZeroBitsIsEmpty();
    testHiMaskAllBitsIsFull();
    testHiMaskSaturatesBeyondTypeWidth();
  } catch (std::exception const&) {
    std::exit(1); // NOLINT(*-mt-unsafe)
  }
}

// NOLINTEND(*-magic-numbers)
