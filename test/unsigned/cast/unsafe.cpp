#include <common.hpp>
#include <unsigned.hpp>

#include <cstdlib>
#include <exception>

// NOLINTBEGIN(*-magic-numbers)

namespace {

using namespace big;

void
require(bool condition)
{
  if (!condition) {
    std::abort();
  }
}

/// exhaustive narrowing cast to 4 bits must equal masking with the low 4 bits
void
testExhaustiveNarrowingMasks()
{
  for (U32 value = 0U; value <= 0xFFU; ++value) {
    auto const narrowed = castBigUnsafe<UnsignedBig<4>>(UnsignedBig{ static_cast<U8>(value) });
    require(narrowed == (value & 0xFU));
  }
}

/// widening casts must preserve the value exactly
void
testWideningPreservesValue()
{
  constexpr auto value = UnsignedBig{ static_cast<U8>(0xABU) };
  static_assert(castBigUnsafe<UnsignedBig<16>>(value) == 0xABU);
  static_assert(castBigUnsafe<UnsignedBig<64>>(value) == 0xABU);
}

/// casting to the same width must be the identity
void
testSameWidthIsIdentity()
{
  constexpr auto value = UnsignedBig{ static_cast<U8>(0xABU) };
  static_assert(castBigUnsafe<UnsignedBig<8>>(value) == value);
}

/// narrowing and widening zero must stay zero
void
testZeroIsPreserved()
{
  constexpr auto zero = UnsignedBig<8>{};
  static_assert(castBigUnsafe<UnsignedBig<4>>(zero) == 0U);
  static_assert(castBigUnsafe<UnsignedBig<16>>(zero) == 0U);
}

} // namespace

int
main(int /* argc */, char** /* argv */)
{
  try {
    testExhaustiveNarrowingMasks();
    testWideningPreservesValue();
    testSameWidthIsIdentity();
    testZeroIsPreserved();
  } catch (std::exception const&) {
    std::exit(1); // NOLINT(*-mt-unsafe)
  }
}

// NOLINTEND(*-magic-numbers)
