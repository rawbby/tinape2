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

/// exhaustive signed 8 bit addition evaluated against the built in integer reference
void
testExhaustiveAgainstReference()
{
  for (I32 lhs = -127; lhs <= 127; ++lhs) {
    for (I32 rhs = -127; rhs <= 127; ++rhs) {
      auto const bigLhs = SignedBig{ static_cast<I8>(lhs) };
      auto const bigRhs = SignedBig{ static_cast<I8>(rhs) };
      auto const bigRes = bigLhs + bigRhs;
      require(bigRes == lhs + rhs);
    }
  }
}

/// the result type must gain exactly one carry bit
void
testResultTypeGainsCarryBit()
{
  constexpr auto sum = SignedBig{ U8{ 255U } } + SignedBig<Positive, 1>{ U8{ 1U } };
  static_assert(decltype(sum)::gIsIntData);
  static_assert(decltype(sum)::gBits == 9);
  static_assert(sum == 256U);
}

/// adding zero must be the identity
void
testZeroIsIdentity()
{
  constexpr auto value = SignedBig{ static_cast<I8>(-42) };
  static_assert(value + SignedBig{ static_cast<I8>(0) } == value);
  static_assert(SignedBig{ static_cast<I8>(0) } + value == value);
}

/// adding a value and its negation must yield zero
void
testValuePlusNegationIsZero()
{
  constexpr auto value = SignedBig{ static_cast<I8>(42) };
  static_assert(value + (-value) == 0);
}

/// mixed sign addition must reduce the magnitude
void
testMixedSignAddition()
{
  static_assert(SignedBig{ static_cast<I8>(100) } + SignedBig{ static_cast<I8>(-1) } == 99);
  static_assert(SignedBig{ static_cast<I8>(-100) } + SignedBig{ static_cast<I8>(1) } == -99);
  static_assert(SignedBig{ static_cast<I8>(1) } + SignedBig{ static_cast<I8>(-100) } == -99);
}

/// heterogeneous addition with plain integrals must behave like big addition
void
testHeterogeneousOperands()
{
  constexpr auto value = SignedBig{ static_cast<I8>(-43) };
  static_assert(value + 1 == -42);
  static_assert(1 + value == -42);
}

} // namespace

int
main(int /* argc */, char** /* argv */)
{
  try {
    testExhaustiveAgainstReference();
    testResultTypeGainsCarryBit();
    testZeroIsIdentity();
    testValuePlusNegationIsZero();
    testMixedSignAddition();
    testHeterogeneousOperands();
  } catch (std::exception const&) {
    std::exit(1); // NOLINT(*-mt-unsafe)
  }
}

// NOLINTEND(*-magic-numbers)
