#include <common.hpp>
#include <signed.hpp>
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

/// exhaustive narrowing cast to 4 magnitude bits must mask the absolute value and keep the sign
void
testExhaustiveNarrowingMasksAbs()
{
  for (I32 value = -127; value <= 127; ++value) {
    auto const narrowed = castBigUnsafe<SignedBig<Runtime, 4>>(SignedBig{ static_cast<I8>(value) });
    require(narrowed.abs() == (static_cast<U32>(value < 0 ? -value : value) & 0xFU));
    require(narrowed.sign() == (value >= 0));
  }
}

/// widening casts must preserve value and sign exactly
void
testWideningPreservesValue()
{
  constexpr auto value = SignedBig{ static_cast<I8>(-42) };
  static_assert(castBigUnsafe<SignedBig<Runtime, 16>>(value) == -42);
  static_assert(castBigUnsafe<SignedBig<Runtime, 63>>(value) == -42);
}

/// casting to the same width must be the identity
void
testSameWidthIsIdentity()
{
  constexpr auto value = SignedBig{ static_cast<I8>(-42) };
  static_assert(castBigUnsafe<SignedBig<Runtime, 7>>(value) == value);
}

} // namespace

int
main(int /* argc */, char** /* argv */)
{
  try {
    testExhaustiveNarrowingMasksAbs();
    testWideningPreservesValue();
    testSameWidthIsIdentity();
  } catch (std::exception const&) {
    std::exit(1); // NOLINT(*-mt-unsafe)
  }
}

// NOLINTEND(*-magic-numbers)
