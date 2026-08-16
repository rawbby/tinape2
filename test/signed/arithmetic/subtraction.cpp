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

/// exhaustive signed 8 bit subtraction evaluated against the built in integer reference
void
testExhaustiveAgainstReference()
{
  for (I32 lhs = -127; lhs <= 127; ++lhs) {
    for (I32 rhs = -127; rhs <= 127; ++rhs) {
      auto const difference = SignedBig{ static_cast<I8>(lhs) } - SignedBig{ static_cast<I8>(rhs) };
      require(difference == lhs - rhs);
    }
  }
}

/// subtracting zero must be the identity
void
testZeroIsIdentity()
{
  constexpr auto value = SignedBig{ static_cast<I8>(-42) };
  static_assert(value - SignedBig{ static_cast<I8>(0) } == value);
}

/// subtracting a value from itself must yield zero
void
testSelfSubtractionIsZero()
{
  constexpr auto value = SignedBig{ static_cast<I8>(-100) };
  static_assert(value - value == 0);
}

/// subtraction may go below zero
void
testCrossesZero()
{
  static_assert(SignedBig{ static_cast<I8>(1) } - SignedBig{ static_cast<I8>(2) } == -1);
  static_assert(SignedBig{ static_cast<I8>(-1) } - SignedBig{ static_cast<I8>(-2) } == 1);
}

/// subtracting a negative value must add its magnitude
void
testSubtractingNegativeAdds()
{
  static_assert(SignedBig{ static_cast<I8>(40) } - SignedBig{ static_cast<I8>(-2) } == 42);
}

/// subtraction must invert addition
void
testInvertsAddition()
{
  constexpr auto value = SignedBig{ static_cast<I8>(-42) };
  static_assert(value + 1 - 1 == value);
}

/// heterogeneous subtraction with plain integrals must behave like big subtraction
void
testHeterogeneousOperands()
{
  constexpr auto value = SignedBig{ static_cast<I8>(-41) };
  static_assert(value - 1 == -42);
  static_assert(1 - value == 42);
}

} // namespace

int
main(int /* argc */, char** /* argv */)
{
  try {
    testExhaustiveAgainstReference();
    testZeroIsIdentity();
    testSelfSubtractionIsZero();
    testCrossesZero();
    testSubtractingNegativeAdds();
    testInvertsAddition();
    testHeterogeneousOperands();
  } catch (std::exception const&) {
    std::exit(1); // NOLINT(*-mt-unsafe)
  }
}

// NOLINTEND(*-magic-numbers)
