#include <common.hpp>
#include <signed.hpp>
#include <unsigned.hpp>

#include <cstdlib>
#include <exception>

namespace {
// NOLINTBEGIN(*-magic-numbers)
using namespace big;

void
require(bool condition)
{
  if (!condition) {
    std::abort();
  }
}

/// exhaustive signed 8 bit multiplication evaluated against the built in integer reference

/// exhaustive signed 8 bit multiplication evaluated against the built in integer reference
void
testExhaustiveAgainstReference()
{
  constexpr auto kernel = []<auto lhsBits, auto rhsBits>(Tag<lhsBits>, Tag<rhsBits>) {
    constexpr auto end = static_cast<I32>(gLoMask<U8, 6>);
    constexpr auto beg = -end;
    for (auto lhs = beg; lhs <= end; lhs += 4) {
      for (auto rhs = beg; rhs <= end; rhs += 8) {
        auto const bigLhs = castBigUnsafe<SignedBig<Runtime, Size{ lhsBits }>>(SignedBig{ lhs });
        auto const bigRhs = castBigUnsafe<SignedBig<Runtime, Size{ rhsBits }>>(SignedBig{ rhs });
        require(bigLhs * bigRhs == lhs * rhs);
        require(bigRhs * bigLhs == rhs * lhs);
      }
    }
  };
  kernel(gTag<6>, gTag<6>);
  kernel(gTag<6>, gTag<8>);
  kernel(gTag<8>, gTag<6>);
  kernel(gTag<8>, gTag<8>);
  kernel(gTag<32>, gTag<8>);
  kernel(gTag<8>, gTag<32>);
  kernel(gTag<32>, gTag<32>);
  kernel(gTag<64>, gTag<32>);
  kernel(gTag<32>, gTag<64>);
  kernel(gTag<64>, gTag<64>);
  kernel(gTag<128>, gTag<64>);
  kernel(gTag<64>, gTag<128>);
  kernel(gTag<128>, gTag<128>);
  kernel(gTag<1024>, gTag<128>);
  kernel(gTag<128>, gTag<1024>);
  kernel(gTag<1024>, gTag<1024>);
}

/// the sign of the product must follow the sign rule
void
testSignRule()
{
  using PositiveSigned = SignedBig<Positive, 3>;
  using NegativeSigned = SignedBig<Negative, 3>;
  using RuntimeSigned  = SignedBig<Runtime, 3>;

  static_assert(PositiveSigned{ 3 }.abs() == 3);
  static_assert(NegativeSigned{ 3 }.abs() == 3);
  static_assert(RuntimeSigned{ 3 }.abs() == 3);
  static_assert(RuntimeSigned{ -3 }.abs() == 3);

  static_assert(PositiveSigned{ 3 }.sign());
  static_assert(!NegativeSigned{ 3 }.sign());
  static_assert(RuntimeSigned{ 3 }.sign());
  static_assert(!RuntimeSigned{ -3 }.sign());

  static_assert(PositiveSigned{ 3 } == 3);
  static_assert(NegativeSigned{ 3 } == -3);
  static_assert(RuntimeSigned{ 3 } == 3);
  static_assert(RuntimeSigned{ -3 } == -3);

  static_assert(PositiveSigned{ 3 } != NegativeSigned{ 3 });
  static_assert(PositiveSigned{ 3 } == RuntimeSigned{ 3 });
  static_assert(NegativeSigned{ 3 } != PositiveSigned{ 3 });
  static_assert(NegativeSigned{ 3 } == RuntimeSigned{ -3 });
  static_assert(RuntimeSigned{ 3 } == PositiveSigned{ 3 });
  static_assert(RuntimeSigned{ -3 } == NegativeSigned{ 3 });

  static_assert(PositiveSigned{ 3 }.abs() == NegativeSigned{ 3 }.abs());
  static_assert(PositiveSigned{ 3 }.abs() == RuntimeSigned{ 3 }.abs());
  static_assert(NegativeSigned{ 3 }.abs() == PositiveSigned{ 3 }.abs());
  static_assert(NegativeSigned{ 3 }.abs() == RuntimeSigned{ 3 }.abs());
  static_assert(RuntimeSigned{ 3 }.abs() == PositiveSigned{ 3 }.abs());
  static_assert(RuntimeSigned{ 3 }.abs() == NegativeSigned{ 3 }.abs());

  static_assert(PositiveSigned{ 3 }.abs() == RuntimeSigned{ -3 }.abs());
  static_assert(NegativeSigned{ 3 }.abs() == RuntimeSigned{ -3 }.abs());
  static_assert(RuntimeSigned{ -3 }.abs() == PositiveSigned{ 3 }.abs());
  static_assert(RuntimeSigned{ -3 }.abs() == NegativeSigned{ 3 }.abs());

  static_assert(PositiveSigned{ 3 } * PositiveSigned{ 4 } == 12);
  static_assert(NegativeSigned{ 3 } * PositiveSigned{ 4 } == -12);
  static_assert(PositiveSigned{ 3 } * NegativeSigned{ 4 } == -12);
  static_assert(NegativeSigned{ 3 } * NegativeSigned{ 4 } == 12);

  // Positive -> Runtime
  static_assert(RuntimeSigned{ 3 } * RuntimeSigned{ 4 } == 12);
  static_assert(NegativeSigned{ 3 } * RuntimeSigned{ 4 } == -12);
  static_assert(RuntimeSigned{ 3 } * NegativeSigned{ 4 } == -12);
  static_assert(NegativeSigned{ 3 } * NegativeSigned{ 4 } == 12);

  // Negative -> Runtime
  static_assert(PositiveSigned{ 3 } * PositiveSigned{ 4 } == 12);
  static_assert(RuntimeSigned{ -3 } * PositiveSigned{ 4 } == -12);
  static_assert(PositiveSigned{ 3 } * RuntimeSigned{ -4 } == -12);
  static_assert(RuntimeSigned{ -3 } * RuntimeSigned{ -4 } == 12);

  // Negative -> Runtime
  // Positive -> Runtime
  static_assert(RuntimeSigned{ 3 } * RuntimeSigned{ 4 } == 12);
  static_assert(RuntimeSigned{ -3 } * RuntimeSigned{ 4 } == -12);
  static_assert(RuntimeSigned{ 3 } * RuntimeSigned{ -4 } == -12);
  static_assert(RuntimeSigned{ -3 } * RuntimeSigned{ -4 } == 12);
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

// NOLINTEND(*-magic-numbers)
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
