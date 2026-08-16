#include <common.hpp>

#include <cstdlib>
#include <exception>

// NOLINTBEGIN(*-magic-numbers)

namespace {

/// gLoMask must select the requested number of low bits
void
testLoMaskSelectsLowBits()
{
  static_assert(gLoMask<U8, 1> == 0b00000001);
  static_assert(gLoMask<U8, 3> == 0b00000111);
  static_assert(gLoMask<U8, 7> == 0b01111111);
}

/// zero bits requested must produce an empty mask
void
testLoMaskZeroBitsIsEmpty()
{
  static_assert(gLoMask<U8, 0> == 0b00000000);
}

/// requesting all bits must produce a full mask
void
testLoMaskAllBitsIsFull()
{
  static_assert(gLoMask<U8, 8> == 0b11111111);
}

/// requesting more bits than the type holds must saturate to a full mask
void
testLoMaskSaturatesBeyondTypeWidth()
{
  static_assert(gLoMask<U8, 12> == 0b11111111);
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
