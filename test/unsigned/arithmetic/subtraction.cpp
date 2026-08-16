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

/// exhaustive 8 bit subtraction (lhs >= rhs) evaluated against the built in integer reference
void
testExhaustiveAgainstReference()
{
  for (U32 lhs = 0U; lhs <= 0xFFU; ++lhs) {
    for (U32 rhs = 0U; rhs <= lhs; ++rhs) {
      auto const difference = UnsignedBig{ static_cast<U8>(lhs) } - UnsignedBig{ static_cast<U8>(rhs) };
      require(difference == lhs - rhs);
    }
  }
}

/// subtracting zero must be the identity
void
testZeroIsIdentity()
{
  constexpr auto value = UnsignedBig{ static_cast<U8>(42U) };
  static_assert(value - UnsignedBig<8>{} == value);
}

/// subtracting a value from itself must yield zero
void
testSelfSubtractionIsZero()
{
  constexpr auto value = UnsignedBig{ static_cast<U8>(200U) };
  static_assert(value - value == 0U);
}

/// subtraction must invert addition
void
testInvertsAddition()
{
  constexpr auto value = UnsignedBig{ std::array{
    static_cast<Element>(1),
    static_cast<Element>(2),
    static_cast<Element>(3),
  } };
  static_assert(value + 1 - 1 == value);
}

/// a borrow must propagate through the whole multi element value
void
testMultiElementBorrowPropagation()
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

  static_assert(powerOfTwo - 1 == allOnes);
}

/// heterogeneous subtraction with plain integrals must behave like big subtraction
void
testHeterogeneousOperands()
{
  constexpr auto value = UnsignedBig{ static_cast<U8>(43U) };
  static_assert(value - 1 == 42U);
  static_assert(43 - UnsignedBig{ static_cast<U8>(1U) } == 42U);
}

} // namespace

int
main(int /* argc */, char** /* argv */)
{
  try {
    testExhaustiveAgainstReference();
    testZeroIsIdentity();
    testSelfSubtractionIsZero();
    testInvertsAddition();
    testMultiElementBorrowPropagation();
    testHeterogeneousOperands();
  } catch (std::exception const&) {
    std::exit(1); // NOLINT(*-mt-unsafe)
  }
}

// NOLINTEND(*-magic-numbers)
