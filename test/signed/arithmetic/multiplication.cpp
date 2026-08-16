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
template<Size lhsBits, Size rhsBits>
void
testExhaustiveAgainstReferenceImpl()
{
  for (I32 lhs = -127; lhs <= 127; ++lhs) {
    for (I32 rhs = -127; rhs <= 127; ++rhs) {
      auto const bigLhs     = castBigUnsafe<SignedBig<Runtime, lhsBits>> (SignedBig{ gTag<Runtime>, static_cast<I8>(lhs) });
      auto const bitRhs     = castBigUnsafe<SignedBig<Runtime, rhsBits>> (SignedBig{ gTag<Runtime>, static_cast<I8>(rhs) });
      auto const bigProduct = bigLhs * bitRhs;
      require(bigProduct == lhs * rhs);
    }
  }
}

/// exhaustive signed 8 bit multiplication evaluated against the built in integer reference
void
testExhaustiveAgainstReference()
{
  testExhaustiveAgainstReferenceImpl<16, 8>();
  testExhaustiveAgainstReferenceImpl<8, 16>();
  testExhaustiveAgainstReferenceImpl<16, 16>();

  testExhaustiveAgainstReferenceImpl<32, 16>();
  testExhaustiveAgainstReferenceImpl<16, 32>();
  testExhaustiveAgainstReferenceImpl<32, 32>();

  testExhaustiveAgainstReferenceImpl<64, 32>();
  testExhaustiveAgainstReferenceImpl<32, 64>();
  testExhaustiveAgainstReferenceImpl<64, 64>();

  testExhaustiveAgainstReferenceImpl<128, 64>();
  testExhaustiveAgainstReferenceImpl<64, 128>();
  testExhaustiveAgainstReferenceImpl<128, 128>();

  testExhaustiveAgainstReferenceImpl<256, 128>();
  testExhaustiveAgainstReferenceImpl<128, 256>();
  testExhaustiveAgainstReferenceImpl<256, 256>();

  testExhaustiveAgainstReferenceImpl<512, 256>();
  testExhaustiveAgainstReferenceImpl<256, 512>();
  testExhaustiveAgainstReferenceImpl<512, 512>();

  testExhaustiveAgainstReferenceImpl<1024, 1024>();
  testExhaustiveAgainstReferenceImpl<2048, 1024>();
  testExhaustiveAgainstReferenceImpl<1024, 2048>();
  testExhaustiveAgainstReferenceImpl<2048, 2048>();
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
