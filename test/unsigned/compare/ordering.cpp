#include <common.hpp>
#include <unsigned.hpp>

#include <array>
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

/// exhaustive 8 bit three way comparison evaluated against the built in integer reference
void
testExhaustiveAgainstReference()
{
  for (U32 lhs = 0U; lhs <= 0xFFU; ++lhs) {
    for (U32 rhs = 0U; rhs <= 0xFFU; ++rhs) {
      auto const bigLhs = UnsignedBig{ static_cast<U8>(lhs) };
      auto const bigRhs = UnsignedBig{ static_cast<U8>(rhs) };
      require((bigLhs <=> bigRhs) == (lhs <=> rhs));
    }
  }
}

/// ordering must work across different bit widths
void
testOrderingAcrossWidths()
{
  constexpr auto narrow = UnsignedBig{ static_cast<U8>(255U) };
  constexpr auto wide   = UnsignedBig{ static_cast<U16>(256U) };
  static_assert(narrow < wide);
  static_assert(wide > narrow);
}

/// ordering must respect high elements of multi element values
void
testMultiElementOrdering()
{
  constexpr auto lower  = UnsignedBig{ std::array{
    static_cast<Element>(9),
    static_cast<Element>(2),
    static_cast<Element>(3),
  } };
  constexpr auto higher = UnsignedBig{ std::array{
    static_cast<Element>(1),
    static_cast<Element>(2),
    static_cast<Element>(4),
  } };
  static_assert(lower < higher);
  static_assert(higher > lower);
}

/// derived relational operators must be consistent around a boundary value
void
testDerivedRelationalOperators()
{
  constexpr auto value = UnsignedBig{ static_cast<U16>(256U) };
  static_assert(value < 257U);
  static_assert(value <= 256U);
  static_assert(value > 255U);
  static_assert(value >= 256U);
}

} // namespace

int
main(int /* argc */, char** /* argv */)
{
  try {
    testExhaustiveAgainstReference();
    testOrderingAcrossWidths();
    testMultiElementOrdering();
    testDerivedRelationalOperators();
  } catch (std::exception const&) {
    std::exit(1); // NOLINT(*-mt-unsafe)
  }
}

// NOLINTEND(*-magic-numbers)
