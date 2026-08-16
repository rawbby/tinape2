#include <common.hpp>
#include <unsigned.hpp>

#include <array>
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

/// exhaustive 8 bit equality evaluated against the built in integer reference
void
testExhaustiveAgainstReference()
{
  for (U32 lhs = 0U; lhs <= 0xFFU; ++lhs) {
    for (U32 rhs = 0U; rhs <= 0xFFU; ++rhs) {
      auto const bigLhs = UnsignedBig{ static_cast<U8>(lhs) };
      auto const bigRhs = UnsignedBig{ static_cast<U8>(rhs) };
      require((bigLhs == bigRhs) == (lhs == rhs));
      require((bigLhs != bigRhs) == (lhs != rhs));
    }
  }
}

/// equality must hold across different bit widths for the same value
void
testEqualityAcrossWidths()
{
  static_assert(UnsignedBig{ static_cast<U8>(42U) } == UnsignedBig{ static_cast<U16>(42U) });
  static_assert(UnsignedBig{ static_cast<U8>(42U) } != UnsignedBig{ static_cast<U16>(43U) });
}

/// equality must hold between int data and array data representations
void
testEqualityAcrossRepresentations()
{
  constexpr auto allOnes    = UnsignedBig{ std::array{
    0xFFFFFFFFU,
    0xFFFFFFFFU,
    0xFFFFFFFFU,
    0xFFFFFFFFU,
    0xFFFFFFFFU,
    0xFFFFFFFFU,
    0xFFFFFFFFU,
    0xFFFFFFFFU,
    0xFFFFFFFFU,
  } };
  constexpr auto powerOfTwo = UnsignedBig{ std::array{ 0x0U, 0x0U, 0x0U, 0x0U, 0x0U, 0x0U, 0x0U, 0x0U, 0x0U, 0x1U } };

  static_assert(allOnes + UnsignedBig<1>{ 1 } == powerOfTwo);
  static_assert(allOnes != powerOfTwo);
}

/// heterogeneous equality with plain integrals must behave like big equality
void
testHeterogeneousOperands()
{
  constexpr auto value = UnsignedBig{ static_cast<U8>(42U) };
  static_assert(value == 42U);
  static_assert(42U == value);
  static_assert(value != 43U);
}

} // namespace

int
main(int /* argc */, char** /* argv */)
{
  try {
    testExhaustiveAgainstReference();
    testEqualityAcrossWidths();
    testEqualityAcrossRepresentations();
    testHeterogeneousOperands();
  } catch (std::exception const&) {
    std::exit(1); // NOLINT(*-mt-unsafe)
  }
}

// NOLINTEND(*-magic-numbers)
