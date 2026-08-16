#include <common.hpp>
#include <signed.hpp>
#include <unsigned.hpp>

#include <cstdlib>
#include <exception>
#include <optional>

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

/// exhaustive narrowing cast to 4 magnitude bits must yield the value iff it fits and nullopt otherwise
void
testExhaustiveNarrowing()
{
  for (I32 value = -127; value <= 127; ++value) {
    auto const narrowed = castBigSafe<SignedBig<Runtime, 4>>(SignedBig{ static_cast<I8>(value) });
    if (value >= -15 && value <= 15) {
      require(narrowed.has_value());
      require(*narrowed == value);
    } else {
      require(!narrowed.has_value());
    }
  }
}

/// widening casts must always succeed and preserve value and sign
void
testWideningAlwaysSucceeds()
{
  constexpr auto value   = SignedBig{ static_cast<I8>(-42) };
  constexpr auto widened = castBigSafe<SignedBig<Runtime, 16>>(value);
  static_assert(widened.has_value());
  static_assert(*widened == -42);
}

/// casting to the same width must always succeed and be the identity
void
testSameWidthIsIdentity()
{
  constexpr auto value = SignedBig{ static_cast<I8>(-42) };
  constexpr auto same  = castBigSafe<SignedBig<Runtime, 7>>(value);
  static_assert(same.has_value());
  static_assert(*same == value);
}

/// a negative value must not fit into a positive sign type unless it is zero
void
testNegativeIntoPositiveSignType()
{
  constexpr auto negative = castBigSafe<SignedBig<Positive, 8>>(SignedBig{ static_cast<I8>(-42) });
  static_assert(!negative.has_value());

  constexpr auto negativeZero = castBigSafe<SignedBig<Positive, 8>>(SignedBig{ static_cast<U8>(0U), false });
  static_assert(negativeZero.has_value());
  static_assert(*negativeZero == 0);
}

} // namespace

int
main(int /* argc */, char** /* argv */)
{
  try {
    testExhaustiveNarrowing();
    testWideningAlwaysSucceeds();
    testSameWidthIsIdentity();
    testNegativeIntoPositiveSignType();
  } catch (std::exception const&) {
    std::exit(1); // NOLINT(*-mt-unsafe)
  }
}

// NOLINTEND(*-magic-numbers)
