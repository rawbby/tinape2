#include <common.hpp>
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

/// exhaustive 8 bit addition evaluated against the built in integer reference
void
testExhaustiveAgainstReference()
{
  for (U32 lhs = 0U; lhs < 256; ++lhs) {
    for (U32 rhs = 0U; rhs < 256; ++rhs) {
      auto const bigLhs = UnsignedBig{ static_cast<U8>(lhs) };
      auto const bigRhs = UnsignedBig{ static_cast<U8>(rhs) };
      auto const bigRL  = bigLhs + bigRhs;
      auto const bigLR  = bigLhs + bigRhs;
      require(bigRL == rhs + lhs);
      require(bigLR == lhs + rhs);
    }
  }
}

/// the result type must gain exactly one carry bit
void
testResultTypeGainsCarryBit()
{
  constexpr auto sum = UnsignedBig{ static_cast<U8>(255U) } + UnsignedBig<1>{ static_cast<U8>(1U) };
  static_assert(decltype(sum)::gIsIntData);
  static_assert(decltype(sum)::gBits == 9);
  static_assert(sum.mData == 256U);
}

/// adding zero must be the identity
void
testZeroIsIdentity()
{
  constexpr auto value = UnsignedBig{ static_cast<U8>(42U) };
  static_assert(value + UnsignedBig<8>{} == value);
  static_assert(UnsignedBig<8>{} + value == value);
}

/// addition must be commutative
void
testCommutativity()
{
  for (U32 lhs = 0U; lhs <= 256; lhs += 3U) {
    for (U32 rhs = 0U; rhs <= 256; rhs += 7U) {
      auto const bigLhs = UnsignedBig{ static_cast<U8>(lhs) };
      auto const bigRhs = UnsignedBig{ static_cast<U8>(rhs) };
      auto const bigLR  = bigLhs + bigRhs;
      auto const bigRL  = bigRhs + bigLhs;
      require(bigLR == bigRL);
    }
  }
}

/// a carry must propagate through the whole multi element value
void
testMultiElementCarryPropagation()
{
  constexpr auto mask     = gMask<Limb>;
  constexpr auto allOnes  = UnsignedBig{ Limbs<9>{ mask, mask, mask, mask, mask, mask, mask, mask, mask } };
  constexpr auto expected = UnsignedBig{ Limbs<10>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 } };
  constexpr auto sum      = allOnes + gUnsignedBig<1>;

  static_assert(decltype(sum)::gIsArrayData);
  static_assert(decltype(sum)::gBits == (9 * gBitsOf<U32>)+1);
  static_assert(sum.element(0) == 0U);
  static_assert(sum.element(1) == 0U);
  static_assert(sum == expected);
}

/// heterogeneous addition with plain integrals must behave like big addition
void
testHeterogeneousOperands()
{
  constexpr auto value = UnsignedBig{ static_cast<U8>(41U) };
  static_assert(value + 1 == 42U);
  static_assert(1 + value == 42U);
}

} // namespace

int
main(int /* argc */, char** /* argv */)
{
  try {
    testExhaustiveAgainstReference();
    testResultTypeGainsCarryBit();
    testZeroIsIdentity();
    testCommutativity();
    testMultiElementCarryPropagation();
    testHeterogeneousOperands();
  } catch (std::exception const&) {
    std::exit(1); // NOLINT(*-mt-unsafe)
  }
}

// NOLINTEND(*-magic-numbers)
