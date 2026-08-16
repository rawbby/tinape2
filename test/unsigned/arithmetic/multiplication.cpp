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

/// exhaustive 8 bit multiplication evaluated against the built in integer reference
void
testExhaustiveAgainstReference()
{
  for (U32 lhs = 0U; lhs <= 0xFFU; ++lhs) {
    for (U32 rhs = 0U; rhs <= 0xFFU; ++rhs) {
      auto const product = UnsignedBig{ static_cast<U8>(lhs) } * UnsignedBig{ static_cast<U8>(rhs) };
      require(product == lhs * rhs);
    }
  }
}

/// the result type must be the sum of the operand widths
void
testResultTypeIsSumOfWidths()
{
  constexpr auto product = UnsignedBig{ static_cast<U8>(255U) } * UnsignedBig{ static_cast<U8>(255U) };
  static_assert(decltype(product)::gBits == 16);
  static_assert(product == 255U * 255U);
}

/// multiplication by zero must yield zero
void
testZeroAnnihilates()
{
  constexpr auto value = UnsignedBig{ static_cast<U8>(42U) };
  static_assert(value * UnsignedBig<8>{} == 0U);
  static_assert(UnsignedBig<8>{} * value == 0U);
}

/// multiplication by one must be the identity
void
testOneIsIdentity()
{
  constexpr auto value = UnsignedBig{ static_cast<U8>(42U) };
  static_assert(value * UnsignedBig<1>{ 1 } == value);
  static_assert(UnsignedBig<1>{ 1 } * value == value);
}

/// multiplication must be commutative
void
testCommutativity()
{
  for (U32 lhs = 0U; lhs <= 0xFFU; lhs += 3U) {
    for (U32 rhs = 0U; rhs <= 0xFFU; rhs += 7U) {
      auto const bigLhs = UnsignedBig{ static_cast<U8>(lhs) };
      auto const bigRhs = UnsignedBig{ static_cast<U8>(rhs) };
      require(bigLhs * bigRhs == bigRhs * bigLhs);
    }
  }
}

/// multi element multiplication must distribute over addition
void
testMultiElementDistributivity()
{
  constexpr auto valueA = UnsignedBig{ std::array{
    static_cast<Element>(1),
    static_cast<Element>(2),
    static_cast<Element>(3),
  } };
  constexpr auto valueB = UnsignedBig{ std::array{
    static_cast<Element>(5),
    static_cast<Element>(7),
    static_cast<Element>(11),
  } };
  constexpr auto valueC = UnsignedBig{ static_cast<U8>(13U) };

  static_assert((valueA + valueB) * valueC == valueA * valueC + valueB * valueC);
}

/// heterogeneous multiplication with plain integrals must behave like big multiplication
void
testHeterogeneousOperands()
{
  constexpr auto value = UnsignedBig{ static_cast<U8>(21U) };
  static_assert(value * 2 == 42U);
  static_assert(2 * value == 42U);
}

} // namespace

int
main(int /* argc */, char** /* argv */)
{
  try {
    testExhaustiveAgainstReference();
    testResultTypeIsSumOfWidths();
    testZeroAnnihilates();
    testOneIsIdentity();
    testCommutativity();
    testMultiElementDistributivity();
    testHeterogeneousOperands();
  } catch (std::exception const&) {
    std::exit(1); // NOLINT(*-mt-unsafe)
  }
}

// NOLINTEND(*-magic-numbers)
