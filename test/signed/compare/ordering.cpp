#include <common.hpp>
#include <signed.hpp>

#include <compare>
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

/// exhaustive signed 8 bit three way comparison evaluated against the built in integer reference
void
testExhaustiveAgainstReference()
{
  for (I32 lhs = -127; lhs <= 127; ++lhs) {
    for (I32 rhs = -127; rhs <= 127; ++rhs) {
      auto const bigLhs = SignedBig{ static_cast<I8>(lhs) };
      auto const bigRhs = SignedBig{ static_cast<I8>(rhs) };
      require((bigLhs <=> bigRhs) == (lhs <=> rhs));
    }
  }
}

/// any negative value must order below any positive value
void
testNegativeBelowPositive()
{
  static_assert(SignedBig{ static_cast<I8>(-1) } < SignedBig{ static_cast<I8>(1) });
  static_assert(SignedBig{ static_cast<I8>(-127) } < SignedBig{ static_cast<I8>(127) });
}

/// negative values must order reversed by magnitude
void
testNegativeMagnitudeOrdering()
{
  static_assert(SignedBig{ static_cast<I8>(-2) } < SignedBig{ static_cast<I8>(-1) });
  static_assert(SignedBig{ static_cast<I8>(-1) } > SignedBig{ static_cast<I8>(-2) });
}

/// positive and negative zero must compare equal to zero
void
testZeroSignIsIrrelevant()
{
  constexpr auto positiveZero = SignedBig{ static_cast<U8>(0U), true };
  constexpr auto negativeZero = SignedBig{ static_cast<U8>(0U), false };
  static_assert(!(positiveZero < negativeZero));
  static_assert(!(negativeZero < positiveZero));
  static_assert((positiveZero <=> negativeZero) == std::strong_ordering::equal);
}

/// derived relational operators must be consistent around a boundary value
void
testDerivedRelationalOperators()
{
  constexpr auto value = SignedBig{ static_cast<I16>(256) };
  static_assert(value < 257);
  static_assert(value <= 256);
  static_assert(value > 255);
  static_assert(value >= 256);
}

} // namespace

int
main(int /* argc */, char** /* argv */)
{
  try {
    testExhaustiveAgainstReference();
    testNegativeBelowPositive();
    testNegativeMagnitudeOrdering();
    testZeroSignIsIrrelevant();
    testDerivedRelationalOperators();
  } catch (std::exception const&) {
    std::exit(1); // NOLINT(*-mt-unsafe)
  }
}

// NOLINTEND(*-magic-numbers)
