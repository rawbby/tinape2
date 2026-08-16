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
  static_assert(gHiMask<U8, 3> == 0b11100000);
  static_assert(gHiMask<U8, 7> == 0b11111110);
}

/// zero bits requested must produce an empty mask
void
testHiMaskZeroBitsIsEmpty()
{
  static_assert(gHiMask<U8, 0> == 0b00000000);
}

/// requesting all bits must produce a full mask
void
testHiMaskAllBitsIsFull()
{
  static_assert(gHiMask<U8, 8> == 0b11111111);
}

/// requesting more bits than the type holds must saturate to a full mask
void
testHiMaskSaturatesBeyondTypeWidth()
{
  static_assert(gHiMask<U8, 12> == 0b11111111);
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
