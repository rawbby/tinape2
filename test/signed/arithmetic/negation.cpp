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

/// exhaustive negation evaluated against the built in integer reference
void
testExhaustiveAgainstReference()
{
  for (I32 value = -127; value <= 127; ++value) {
    auto const negated = -SignedBig{ static_cast<I8>(value) };
    require(negated == -value);
  }
}

/// negation must flip the sign and keep the absolute value
void
testFlipsSignKeepsAbs()
{
  constexpr auto value   = SignedBig{ static_cast<I8>(7) };
  constexpr auto negated = -value;
  static_assert(negated.abs() == value.abs());
  static_assert(negated.sign() != value.sign());
}

/// double negation must be the identity
void
testDoubleNegationIsIdentity()
{
  constexpr auto value = SignedBig{ static_cast<I8>(-42) };
  static_assert(-(-value) == value);
}

/// negating a positive sign type must yield a negative sign type
void
testNegatesPositiveSignType()
{
  constexpr auto positive = SignedBig{ static_cast<U8>(42U) };
  static_assert(decltype(positive)::gSignType == Positive);

  constexpr auto negated = -positive;
  static_assert(negated == -42);
}

} // namespace

int
main(int /* argc */, char** /* argv */)
{
  try {
    testExhaustiveAgainstReference();
    testFlipsSignKeepsAbs();
    testDoubleNegationIsIdentity();
    testNegatesPositiveSignType();
  } catch (std::exception const&) {
    std::exit(1); // NOLINT(*-mt-unsafe)
  }
}

// NOLINTEND(*-magic-numbers)
