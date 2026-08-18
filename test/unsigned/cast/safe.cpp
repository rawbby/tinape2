#include <common.hpp>
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

/// exhaustive narrowing cast to 4 bits must yield the value iff it fits and nullopt otherwise
void
testExhaustiveNarrowing()
{
  for (U32 value = 0U; value <= 0xFFU; ++value) {
    auto const narrowed = castBigSafe<UnsignedBig<4>>(UnsignedBig{ static_cast<U8>(value) });
    if (value <= 0xFU) {
      require(narrowed.has_value());
      require(narrowed.value() == value); // NOLINT(*-unchecked-optional-access)
    } else {
      require(!narrowed.has_value());
    }
  }
}

/// widening casts must always succeed and preserve the value
void
testWideningAlwaysSucceeds()
{
  constexpr auto value   = UnsignedBig{ static_cast<U8>(0xABU) };
  constexpr auto widened = castBigSafe<UnsignedBig<16>>(value);
  static_assert(widened.has_value());
  static_assert(*widened == 0xABU);
}

/// casting to the same width must always succeed and be the identity
void
testSameWidthIsIdentity()
{
  constexpr auto value = UnsignedBig{ static_cast<U8>(0xABU) };
  constexpr auto same  = castBigSafe<UnsignedBig<8>>(value);
  static_assert(same.has_value());
  static_assert(*same == value);
}

/// the boundary value that exactly fits must succeed, one above must fail
void
testNarrowingBoundary()
{
  constexpr auto fits     = castBigSafe<UnsignedBig<4>>(UnsignedBig{ static_cast<U8>(0xFU) });
  constexpr auto overflow = castBigSafe<UnsignedBig<4>>(UnsignedBig{ static_cast<U8>(0x10U) });
  static_assert(fits.has_value());
  static_assert(*fits == 0xFU);
  static_assert(!overflow.has_value());
}

} // namespace

int
main(int /* argc */, char** /* argv */)
{
  try {
    testExhaustiveNarrowing();
    testWideningAlwaysSucceeds();
    testSameWidthIsIdentity();
    testNarrowingBoundary();
  } catch (std::exception const&) {
    std::exit(1); // NOLINT(*-mt-unsafe)
  }
}

// NOLINTEND(*-magic-numbers)
