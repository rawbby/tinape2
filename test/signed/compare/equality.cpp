#include <common.hpp>
#include <signed.hpp>

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

/// exhaustive signed 8 bit equality evaluated against the built in integer reference
void
testExhaustiveAgainstReference()
{
  for (I32 lhs = -127; lhs <= 127; ++lhs) {
    for (I32 rhs = -127; rhs <= 127; ++rhs) {
      auto const bigLhs = SignedBig{ static_cast<I8>(lhs) };
      auto const bigRhs = SignedBig{ static_cast<I8>(rhs) };
      require((bigLhs == bigRhs) == (lhs == rhs));
      require((bigLhs != bigRhs) == (lhs != rhs));
    }
  }
}

/// equal magnitude with different signs must not be equal
void
testSignDistinguishesValues()
{
  static_assert(SignedBig{ static_cast<I8>(42) } != SignedBig{ static_cast<I8>(-42) });
}

/// positive and negative zero must be equal
void
testZeroSignIsIrrelevant()
{
  constexpr auto positiveZero = SignedBig{ static_cast<U8>(0U), true };
  constexpr auto negativeZero = SignedBig{ static_cast<U8>(0U), false };
  static_assert(positiveZero == negativeZero);
}

/// equality must hold across different bit widths for the same value
void
testEqualityAcrossWidths()
{
  static_assert(SignedBig{ static_cast<I8>(-42) } == SignedBig{ static_cast<I16>(-42) });
  static_assert(SignedBig{ static_cast<I8>(-42) } != SignedBig{ static_cast<I16>(-43) });
}

/// heterogeneous equality with plain integrals must behave like big equality
void
testHeterogeneousOperands()
{
  constexpr auto value = SignedBig{ static_cast<I8>(-42) };
  static_assert(value == -42);
  static_assert(-42 == value);
  static_assert(value != 42);
}

} // namespace

int
main(int /* argc */, char** /* argv */)
{
  try {
    testExhaustiveAgainstReference();
    testSignDistinguishesValues();
    testZeroSignIsIrrelevant();
    testEqualityAcrossWidths();
    testHeterogeneousOperands();
  } catch (std::exception const&) {
    std::exit(1); // NOLINT(*-mt-unsafe)
  }
}

// NOLINTEND(*-magic-numbers)
