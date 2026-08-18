#include <common.hpp>
#include <restricted.hpp>
#include <signed.hpp>
#include <unsigned.hpp>

#include <cstdlib>
#include <exception>
#include <type_traits>

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

void
testNaiveMode()
{
  // degenerate zero range: [0, 0]
  using ZeroTraits = impl::IsRestrictedBigHelper<gSignedBig<0>, gSignedBig<0>>;
  static_assert(ZeroTraits::gMode == RestrictedMode::Naive);
  static_assert(ZeroTraits::gSignType == SignType::Positive);
  static_assert(ZeroTraits::gSpan == 0);
  static_assert(ZeroTraits::gSpanBits == 0);
  static_assert(ZeroTraits::gMagBits == 0);
  static_assert(ZeroTraits::gBits == 0);
  static_assert(ZeroTraits::gBias == 0);
  static_assert(std::is_same_v<ZeroTraits::Storage, SignedBig<Positive, 0>>);
  static_assert(ZeroTraits::gBits <= ZeroTraits::gMagBits);

  // narrow positive range: [5, 10]
  using PosTraits = impl::IsRestrictedBigHelper<gSignedBig<5>, gSignedBig<10>>;
  static_assert(PosTraits::gMode == RestrictedMode::Naive);
  static_assert(PosTraits::gSignType == SignType::Positive);
  static_assert(PosTraits::gSpan == 5);
  static_assert(PosTraits::gSpanBits == 3);
  static_assert(PosTraits::gMagBits == 4);
  static_assert(PosTraits::gBits == 4);
  static_assert(PosTraits::gBias == 0);
  static_assert(std::is_same_v<PosTraits::Storage, SignedBig<Positive, 4>>);
  static_assert(PosTraits::gBits <= PosTraits::gMagBits);

  // narrow negative range: [-10, -5]
  using NegTraits = impl::IsRestrictedBigHelper<gSignedBig<-10>, gSignedBig<-5>>;
  static_assert(NegTraits::gMode == RestrictedMode::Naive);
  static_assert(NegTraits::gSignType == SignType::Negative);
  static_assert(NegTraits::gSpan == 5);
  static_assert(NegTraits::gSpanBits == 3);
  static_assert(NegTraits::gMagBits == 4);
  static_assert(NegTraits::gBits == 4);
  static_assert(NegTraits::gBias == 0);
  static_assert(std::is_same_v<NegTraits::Storage, SignedBig<Negative, 4>>);
  static_assert(NegTraits::gBits <= NegTraits::gMagBits);

  // narrow straddling zero: [-5, 10]
  using StraddleTraits = impl::IsRestrictedBigHelper<gSignedBig<-5>, gSignedBig<10>>;
  static_assert(StraddleTraits::gMode == RestrictedMode::Naive);
  static_assert(StraddleTraits::gSignType == SignType::Runtime);
  static_assert(StraddleTraits::gSpan == 15);
  static_assert(StraddleTraits::gSpanBits == 4);
  static_assert(StraddleTraits::gMagBits == 4);
  static_assert(StraddleTraits::gBits == 4);
  static_assert(StraddleTraits::gBias == 0);
  static_assert(std::is_same_v<StraddleTraits::Storage, SignedBig<Runtime, 4>>);
  static_assert(StraddleTraits::gBits <= StraddleTraits::gMagBits);

  // boundary case at gWideBits
  constexpr auto maxWidePos = gSignedBig<gMask<Wide>>;
  using WideBoundaryTraits  = impl::IsRestrictedBigHelper<gSignedBig<0>, maxWidePos>;
  static_assert(WideBoundaryTraits::gMagBits == gWideBits);
  static_assert(WideBoundaryTraits::gMode == RestrictedMode::Naive);
  static_assert(WideBoundaryTraits::gBits == gWideBits);
  static_assert(std::is_same_v<WideBoundaryTraits::Storage, SignedBig<Positive, gWideBits>>);
}

void
testExactShiftMode()
{
  // multi-element bounds with small span (positive)
  constexpr auto posBase = SignedBig<Positive, 3 * gElementBits>{ Elements<3>{ 10, 0, 1 } };
  constexpr auto posLb   = gSignedBig<posBase>;
  constexpr auto posUb   = gSignedBig<posBase + 10>;

  using ExactPosTraits = impl::IsRestrictedBigHelper<posLb, posUb>;
  static_assert(ExactPosTraits::gMagBits > gWideBits);
  static_assert(ExactPosTraits::gMode == RestrictedMode::ExactShift);
  static_assert(ExactPosTraits::gSignType == SignType::Positive);
  static_assert(ExactPosTraits::gSpan == 10);
  static_assert(ExactPosTraits::gSpanBits == 4);
  static_assert(ExactPosTraits::gBias == posLb);
  static_assert(ExactPosTraits::gBits == 4);
  static_assert(std::is_same_v<ExactPosTraits::Storage, UnsignedBig<4>>);
  static_assert(ExactPosTraits::gBits <= ExactPosTraits::gMagBits);

  // multi-element bounds with small span (negative)
  constexpr auto negBase = SignedBig<Negative, 3 * gElementBits>{ Elements<3>{ 20, 0, 1 } };
  constexpr auto negLb   = gSignedBig<negBase>;
  constexpr auto negUb   = gSignedBig<negBase + 10>;

  using ExactNegTraits = impl::IsRestrictedBigHelper<negLb, negUb>;
  static_assert(ExactNegTraits::gMagBits > gWideBits);
  static_assert(ExactNegTraits::gMode == RestrictedMode::ExactShift);
  static_assert(ExactNegTraits::gSignType == SignType::Negative);
  static_assert(ExactNegTraits::gSpan == 10);
  static_assert(ExactNegTraits::gSpanBits == 4);
  static_assert(ExactNegTraits::gBias == negLb);
  static_assert(ExactNegTraits::gBits == 4);
  static_assert(std::is_same_v<ExactNegTraits::Storage, UnsignedBig<4>>);
  static_assert(ExactNegTraits::gBits <= ExactNegTraits::gMagBits);

  // degenerate multi-element bound: lb == ub
  using ExactDegenerateTraits = impl::IsRestrictedBigHelper<posLb, posLb>;
  static_assert(ExactDegenerateTraits::gMode == RestrictedMode::ExactShift);
  static_assert(ExactDegenerateTraits::gSpanBits == 0);
  static_assert(ExactDegenerateTraits::gBits == 0);
  static_assert(std::is_same_v<ExactDegenerateTraits::Storage, UnsignedBig<0>>);
  static_assert(ExactDegenerateTraits::gBias == posLb);
}

void
testElementShiftMode()
{
  // multi-element bounds with span exceeding gWideBits
  constexpr auto posBase   = SignedBig<Positive, 4 * gElementBits>{ Elements<4>{ 15, 0, 0, 1 } };
  constexpr auto largeSpan = SignedBig<Positive, 3 * gElementBits>{ Elements<3>{ 1, 1, 1 } };
  constexpr auto elemPosLb = gSignedBig<posBase>;
  constexpr auto elemPosUb = gSignedBig<posBase + largeSpan>;

  using ElemPosTraits = impl::IsRestrictedBigHelper<elemPosLb, elemPosUb>;
  static_assert(ElemPosTraits::gMagBits > gWideBits);
  static_assert(ElemPosTraits::gSpanBits > gWideBits);
  static_assert(ElemPosTraits::gMode == RestrictedMode::ElementShift);
  static_assert(ElemPosTraits::gSignType == SignType::Positive);
  static_assert(elemPosLb - ElemPosTraits::gBias >= 0);
  static_assert(ElemPosTraits::gBits % gElementBits == 0);

  // large bounds straddling zero (sign(lb) != sign(ub))
  constexpr auto negBase   = SignedBig<Negative, 3 * gElementBits>{ Elements<3>{ 10, 0, 1 } };
  constexpr auto elemNegLb = gSignedBig<negBase>;
  constexpr auto elemNegUb = gSignedBig<posBase>;

  using ElemStraddleTraits = impl::IsRestrictedBigHelper<elemNegLb, elemNegUb>;
  static_assert(ElemStraddleTraits::gMagBits > gWideBits);
  static_assert(ElemStraddleTraits::gMode == RestrictedMode::ElementShift);
  static_assert(ElemStraddleTraits::gSignType == SignType::Runtime);
  static_assert(elemNegLb - ElemStraddleTraits::gBias >= 0);
  static_assert(ElemStraddleTraits::gBits % gElementBits == 0);
}

void
testConceptsAndCommon()
{
  using R1 = RestrictedBig<gSignedBig<-5>, gSignedBig<10>>;
  using R2 = RestrictedBig<gSignedBig<0>, gSignedBig<20>>;

  static_assert(IsRestrictedBig<R1>);
  static_assert(IsRestrictedBig<R2>);
  static_assert(IsRestrictedBig<R1 const&>);
  static_assert(!IsRestrictedBig<int>);
  static_assert(!IsRestrictedBig<SignedBig<Positive, 5>>);
  static_assert(!IsRestrictedBig<UnsignedBig<10>>);

  using Common = CommonRestrictedBig<R1, R2>;
  static_assert(Common::gLb == -5);
  static_assert(Common::gUb == 20);
  static_assert(Common::gSignType == SignType::Runtime);
  static_assert(Common::gMode == RestrictedMode::Naive);

  using R3        = RestrictedBig<gSignedBig<10>, gSignedBig<30>>;
  using CommonPos = CommonRestrictedBig<R2, R3>;
  static_assert(CommonPos::gLb == 0);
  static_assert(CommonPos::gUb == 30);
  static_assert(CommonPos::gSignType == SignType::Positive);
}

void
testNonCanonicalBounds()
{
  // Bounds with runtime sign type and oversized bit widths
  constexpr auto nonCanonicalLb = SignedBig<Runtime, 64>{ -5 };
  constexpr auto nonCanonicalUb = SignedBig<Runtime, 64>{ 10 };

  using NonCanonTraits = impl::IsRestrictedBigHelper<nonCanonicalLb, nonCanonicalUb>;
  static_assert(NonCanonTraits::gMode == RestrictedMode::Naive);
  static_assert(NonCanonTraits::gSignType == SignType::Runtime);
  static_assert(NonCanonTraits::gSpan == 15);
  static_assert(NonCanonTraits::gSpanBits == 4);
  static_assert(NonCanonTraits::gMagBits == 4);
  static_assert(NonCanonTraits::gBits == 4);

  using NonCanonRestricted = RestrictedBig<nonCanonicalLb, nonCanonicalUb>;
  static_assert(IsRestrictedBig<NonCanonRestricted>);
  static_assert(NonCanonRestricted::gLb == -5);
  static_assert(NonCanonRestricted::gUb == 10);
  static_assert(NonCanonRestricted::gBits == 4);

  // CommonRestrictedBig with mixed canonical and non-canonical bounds
  using CanonR = RestrictedBig<gSignedBig<0>, gSignedBig<20>>;
  using Common = CommonRestrictedBig<NonCanonRestricted, CanonR>;
  static_assert(Common::gLb == -5);
  static_assert(Common::gUb == 20);
}

void
testRuntimeVerification()
{
  using R = RestrictedBig<gSignedBig<-5>, gSignedBig<10>>;
  require(R::gLb == -5);
  require(R::gUb == 10);
  require(R::gSignType == SignType::Runtime);
  require(R::gMode == RestrictedMode::Naive);
}

} // namespace

int
main(int /* argc */, char** /* argv */)
{
  try {
    testNaiveMode();
    testExactShiftMode();
    testElementShiftMode();
    testConceptsAndCommon();
    testNonCanonicalBounds();
    testRuntimeVerification();
  } catch (std::exception const&) {
    std::exit(1); // NOLINT(*-mt-unsafe)
  }
}

// NOLINTEND(*-magic-numbers)
