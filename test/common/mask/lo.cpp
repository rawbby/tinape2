#include <common.hpp>

#include <cstdlib>
#include <exception>

// NOLINTBEGIN(*-magic-numbers)

namespace {

/// gLoMask must select the requested number of low bits
void
testLoMaskSelectsLowBits()
{
  static_assert(gLoMask<U8, 1> == 1);
  static_assert(gLoMask<U8, 2> == 0b11);
  static_assert(gLoMask<U8, 3> == 0b111);
  static_assert(gLoMask<U8, 4> == 0b1111);
  static_assert(gLoMask<U8, 5> == 0b11111);
  static_assert(gLoMask<U8, 6> == 0b111111);
  static_assert(gLoMask<U8, 7> == 0b1111111);

  static_assert(gLoMask<U16, 1> == 1);
  static_assert(gLoMask<U16, 7> == 0b1111111);
  static_assert(gLoMask<U16, 15> == 0b111111111111111);

  static_assert(gLoMask<U32, 1> == 1);
  static_assert(gLoMask<U32, 7> == 0b1111111);
  static_assert(gLoMask<U32, 31> == 0b1111111111111111111111111111111);

  static_assert(gLoMask<U64, 1> == 1);
  static_assert(gLoMask<U64, 7> == 0b1111111);
  static_assert(gLoMask<U64, 63> == 0b111111111111111111111111111111111111111111111111111111111111111);
}

/// zero bits requested must produce an empty mask
void
testLoMaskZeroBitsIsEmpty()
{
  static_assert(gLoMask<U8, 0> == 0);
  static_assert(gLoMask<U16, 0> == 0);
  static_assert(gLoMask<U32, 0> == 0);
  static_assert(gLoMask<U64, 0> == 0);
}

/// requesting all bits must produce a full mask
void
testLoMaskAllBitsIsFull()
{
  static_assert(gLoMask<U8, 8> == 0b11111111);
  static_assert(gLoMask<U16, 16> == 0b1111111111111111);
  static_assert(gLoMask<U32, 32> == 0b11111111111111111111111111111111);
  static_assert(gLoMask<U64, 64> == 0b1111111111111111111111111111111111111111111111111111111111111111);
}

/// requesting more bits than the type holds must saturate to a full mask
void
testLoMaskSaturatesBeyondTypeWidth()
{
  static_assert(gLoMask<U8, 9> == 0b11111111);
  static_assert(gLoMask<U8, 10> == 0b11111111);
  static_assert(gLoMask<U16, 17> == 0b1111111111111111);
  static_assert(gLoMask<U16, 18> == 0b1111111111111111);
  static_assert(gLoMask<U32, 33> == 0b11111111111111111111111111111111);
  static_assert(gLoMask<U32, 34> == 0b11111111111111111111111111111111);
  static_assert(gLoMask<U64, 65> == 0b1111111111111111111111111111111111111111111111111111111111111111);
  static_assert(gLoMask<U64, 66> == 0b1111111111111111111111111111111111111111111111111111111111111111);
}

} // namespace

int
main(int /* argc */, char** /* argv */)
{
  try {
    testLoMaskSelectsLowBits();
    testLoMaskZeroBitsIsEmpty();
    testLoMaskAllBitsIsFull();
    testLoMaskSaturatesBeyondTypeWidth();
  } catch (std::exception const&) {
    std::exit(1); // NOLINT(*-mt-unsafe)
  }
}

// NOLINTEND(*-magic-numbers)
