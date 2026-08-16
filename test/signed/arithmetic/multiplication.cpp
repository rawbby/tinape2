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

/// exhaustive signed 8 bit multiplication evaluated against the built in integer reference
void
testExhaustiveAgainstReference()
{
  for (I32 lhs = -127; lhs <= 127; ++lhs) {
    for (I32 rhs = -127; rhs <= 127; ++rhs) {
      auto const product = SignedBig{ static_cast<I8>(lhs) } * SignedBig{ static_cast<I8>(rhs) };
      require(product == lhs * rhs);
    }
  }
}

/// the sign of the product must follow the sign rule
void
testSignRule()
{
  static_assert(SignedBig{ static_cast<I8>(3) } * SignedBig{ static_cast<I8>(4) } == 12);
  static_assert(SignedBig{ static_cast<I8>(-3) } * SignedBig{ static_cast<I8>(4) } == -12);
  static_assert(SignedBig{ static_cast<I8>(3) } * SignedBig{ static_cast<I8>(-4) } == -12);
  static_assert(SignedBig{ static_cast<I8>(-3) } * SignedBig{ static_cast<I8>(-4) } == 12);
}

/// multiplication by zero must yield zero regardless of sign
void
testZeroAnnihilates()
{
  constexpr auto zero = SignedBig{ static_cast<I8>(0) };
  static_assert(SignedBig{ static_cast<I8>(42) } * zero == 0);
  static_assert(SignedBig{ static_cast<I8>(-42) } * zero == 0);
}

/// multiplication by one must be the identity, by minus one the negation
void
testUnitFactors()
{
  constexpr auto value = SignedBig{ static_cast<I8>(-42) };
  static_assert(value * SignedBig{ static_cast<I8>(1) } == value);
  static_assert(value * SignedBig{ static_cast<I8>(-1) } == -value);
}

/// heterogeneous multiplication with plain integrals must behave like big multiplication
void
testHeterogeneousOperands()
{
  constexpr auto value = SignedBig{ static_cast<I8>(-21) };
  static_assert(value * 2 == -42);
  static_assert(2 * value == -42);
}

} // namespace

int
main(int /* argc */, char** /* argv */)
{
  try {
    testExhaustiveAgainstReference();
    testSignRule();
    testZeroAnnihilates();
    testUnitFactors();
    testHeterogeneousOperands();
  } catch (std::exception const&) {
    std::exit(1); // NOLINT(*-mt-unsafe)
  }
}

// NOLINTEND(*-magic-numbers)
