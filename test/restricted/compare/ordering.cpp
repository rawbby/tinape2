#include <common.hpp>
#include <restricted.hpp>
#include <signed.hpp>
#include <unsigned.hpp>

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

/// exhaustive small cross-products against SignedBig comparison
void
testExhaustiveAgainstReference()
{
  using R1 = RestrictedBig<gSignedBig<-127>, gSignedBig<127>>;
  using R2 = RestrictedBig<gSignedBig<-200>, gSignedBig<200>>;

  for (I32 lhs = -127; lhs <= 127; ++lhs) {
    for (I32 rhs = -127; rhs <= 127; ++rhs) {
      auto const res1    = R1{ static_cast<I8>(lhs) };
      auto const res2    = R2{ static_cast<I8>(rhs) };
      auto const signed1 = SignedBig{ static_cast<I8>(lhs) };
      auto const signed2 = SignedBig{ static_cast<I8>(rhs) };

      require((res1 <=> res2) == (signed1 <=> signed2));
      require((res1 < res2) == (signed1 < signed2));
      require((res1 <= res2) == (signed1 <= signed2));
      require((res1 > res2) == (signed1 > signed2));
      require((res1 >= res2) == (signed1 >= signed2));
    }
  }
}

/// comparisons across different storage modes (Naive, ExactShift, ElementShift)
void
testCrossModeOrdering()
{
  // ExactShift bounds: multi-element, same sign, small span
  constexpr auto posBase = SignedBig<Positive, 3 * gElementBits>{ Elements<3>{ 10, 0, 1 } };
  constexpr auto posLb   = gSignedBig<posBase>;
  constexpr auto posUb   = gSignedBig<posBase + 10>;
  using ExactR           = RestrictedBig<posLb, posUb>;
  static_assert(ExactR::gMode == RestrictedMode::ExactShift);

  // ElementShift bounds: multi-element straddling zero
  constexpr auto negBase   = SignedBig<Negative, 3 * gElementBits>{ Elements<3>{ 20, 0, 1 } };
  constexpr auto elemNegLb = gSignedBig<negBase>;
  constexpr auto elemPosUb = gSignedBig<posBase + 50>;
  using ElementR           = RestrictedBig<elemNegLb, elemPosUb>;
  static_assert(ElementR::gMode == RestrictedMode::ElementShift);

  // Naive bounds
  using NaiveR = RestrictedBig<gSignedBig<-200>, gSignedBig<200>>;
  static_assert(NaiveR::gMode == RestrictedMode::Naive);

  constexpr auto exactVal5    = ExactR{ posBase + 5 };
  constexpr auto exactVal8    = ExactR{ posBase + 8 };
  constexpr auto elementVal5  = ElementR{ posBase + 5 };
  constexpr auto elementVal10 = ElementR{ posBase + 10 };

  static_assert((exactVal5 <=> exactVal8) == std::strong_ordering::less);
  static_assert(exactVal5 < exactVal8);
  static_assert(exactVal8 > exactVal5);

  static_assert((exactVal5 <=> elementVal5) == std::strong_ordering::equal);
  static_assert(exactVal5 <= elementVal5);
  static_assert(exactVal5 >= elementVal5);

  static_assert((exactVal5 <=> elementVal10) == std::strong_ordering::less);
  static_assert(exactVal5 < elementVal10);
  static_assert(elementVal10 > exactVal5);

  require((exactVal5 <=> exactVal8) == std::strong_ordering::less);
  require(exactVal5 < exactVal8);
  require(exactVal8 > exactVal5);

  require((exactVal5 <=> elementVal5) == std::strong_ordering::equal);
  require(exactVal5 <= elementVal5);
  require(exactVal5 >= elementVal5);

  require((exactVal5 <=> elementVal10) == std::strong_ordering::less);
  require(exactVal5 < elementVal10);
  require(elementVal10 > exactVal5);
}

/// disjoint bounds must fold to constant expressions without runtime work
void
testDisjointBoundsFold()
{
  using RLo = RestrictedBig<gSignedBig<-20>, gSignedBig<-10>>;
  using RHi = RestrictedBig<gSignedBig<5>, gSignedBig<15>>;

  static_assert((RLo::max() <=> RHi::min()) == std::strong_ordering::less);
  static_assert(RLo::max() < RHi::min());
  static_assert(RLo::max() <= RHi::min());
  static_assert(!(RLo::max() > RHi::min()));
  static_assert(!(RLo::max() >= RHi::min()));

  static_assert((RHi::min() <=> RLo::max()) == std::strong_ordering::greater);
  static_assert(RHi::min() > RLo::max());
  static_assert(RHi::min() >= RLo::max());
  static_assert(!(RHi::min() < RLo::max()));
  static_assert(!(RHi::min() <= RLo::max()));

  constexpr auto resLo = RLo{ -15 };
  constexpr auto resHi = RHi{ 10 };
  static_assert(resLo < resHi);
  static_assert(resHi > resLo);
  static_assert((resLo <=> resHi) == std::strong_ordering::less);
  static_assert((resHi <=> resLo) == std::strong_ordering::greater);
}

/// degenerate single-value ranges
void
testDegenerateRanges()
{
  using ZeroR = RestrictedBig<gSignedBig<0>, gSignedBig<0>>;
  using FiveR = RestrictedBig<gSignedBig<5>, gSignedBig<5>>;

  constexpr auto zeroVal = ZeroR{};
  constexpr auto fiveVal = FiveR{ 5 };

  static_assert((zeroVal <=> ZeroR{}) == std::strong_ordering::equal);
  static_assert((zeroVal <=> fiveVal) == std::strong_ordering::less);
  static_assert((fiveVal <=> zeroVal) == std::strong_ordering::greater);
  static_assert(zeroVal < fiveVal);
  static_assert(fiveVal > zeroVal);
}

/// heterogeneous comparisons with plain integrals, SignedBig, UnsignedBig
void
testHeterogeneousOperands()
{
  using R            = RestrictedBig<gSignedBig<-50>, gSignedBig<50>>;
  constexpr auto val = R{ -42 };

  static_assert(val < 0);
  static_assert(val < -41);
  static_assert(val <= -42);
  static_assert(val > -43);
  static_assert(val >= -42);
  static_assert((val <=> -42) == std::strong_ordering::equal);
  static_assert((val <=> 0) == std::strong_ordering::less);

  static_assert(0 > val);
  static_assert(-41 > val);
  static_assert(-42 >= val);
  static_assert(-43 < val);
  static_assert(-42 <= val);
  static_assert((0 <=> val) == std::strong_ordering::greater);

  constexpr auto sVal = SignedBig{ static_cast<I8>(-42) };
  static_assert((val <=> sVal) == std::strong_ordering::equal);
  static_assert((sVal <=> val) == std::strong_ordering::equal);

  constexpr auto unrVal = RestrictedBig<gSignedBig<0>, gSignedBig<100>>{ 50 };
  constexpr auto uVal   = UnsignedBig<8>{ 50 };
  static_assert((unrVal <=> uVal) == std::strong_ordering::equal);
  static_assert((uVal <=> unrVal) == std::strong_ordering::equal);
  static_assert(unrVal < UnsignedBig<8>{ 51 });
  static_assert(unrVal > UnsignedBig<8>{ 49 });
}

} // namespace

int
main(int /* argc */, char** /* argv */)
{
  try {
    testExhaustiveAgainstReference();
    testCrossModeOrdering();
    testDisjointBoundsFold();
    testDegenerateRanges();
    testHeterogeneousOperands();
  } catch (std::exception const&) {
    std::exit(1); // NOLINT(*-mt-unsafe)
  }
}

// NOLINTEND(*-magic-numbers)
