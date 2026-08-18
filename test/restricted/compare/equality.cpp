#include <common.hpp>
#include <restricted.hpp>
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

      require((res1 == res2) == (signed1 == signed2));
      require((res1 != res2) == (signed1 != signed2));
    }
  }
}

/// comparisons across different storage modes (Naive, ExactShift, ElementShift)
void
testCrossModeEquality()
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

  constexpr auto exactVal    = ExactR{ posBase + 5 };
  constexpr auto elementVal  = ElementR{ posBase + 5 };
  constexpr auto elementVal2 = ElementR{ posBase + 6 };

  static_assert(exactVal == elementVal);
  static_assert(exactVal != elementVal2);
  static_assert(elementVal == exactVal);
  static_assert(elementVal2 != exactVal);

  require(exactVal == elementVal);
  require(exactVal != elementVal2);
  require(elementVal == exactVal);
  require(elementVal2 != exactVal);

  // Test equality within same mode (identical bounds)
  constexpr auto exactValCopy = ExactR{ posBase + 5 };
  static_assert(exactVal == exactValCopy);
  static_assert(!(exactVal != exactValCopy));

  constexpr auto exactOther = ExactR{ posBase + 8 };
  static_assert(exactVal != exactOther);
  static_assert(!(exactVal == exactOther));
}

/// disjoint bounds must fold to constant expressions
void
testDisjointBoundsFold()
{
  using RLo = RestrictedBig<gSignedBig<-20>, gSignedBig<-10>>;
  using RHi = RestrictedBig<gSignedBig<5>, gSignedBig<15>>;

  static_assert(RLo::max() != RHi::min());
  static_assert(!(RLo::max() == RHi::min()));
  static_assert(RHi::min() != RLo::max());
  static_assert(!(RHi::min() == RLo::max()));

  constexpr auto resLo = RLo{ -15 };
  constexpr auto resHi = RHi{ 10 };
  static_assert(resLo != resHi);
  static_assert(!(resLo == resHi));
  static_assert(resHi != resLo);
  static_assert(!(resHi == resLo));
}

/// degenerate single-value ranges
void
testDegenerateRanges()
{
  using ZeroR = RestrictedBig<gSignedBig<0>, gSignedBig<0>>;
  using FiveR = RestrictedBig<gSignedBig<5>, gSignedBig<5>>;

  constexpr auto zeroVal = ZeroR{};
  constexpr auto fiveVal = FiveR{ 5 };

  static_assert(zeroVal == ZeroR{});
  static_assert(zeroVal != fiveVal);
  static_assert(fiveVal == FiveR{ 5 });
}

/// heterogeneous equality with plain integrals, SignedBig, UnsignedBig
void
testHeterogeneousOperands()
{
  using R            = RestrictedBig<gSignedBig<-50>, gSignedBig<50>>;
  constexpr auto val = R{ -42 };

  static_assert(val == -42);
  static_assert(-42 == val);
  static_assert(val != 42);
  static_assert(42 != val);

  constexpr auto sVal = SignedBig{ static_cast<I8>(-42) };
  static_assert(val == sVal);
  static_assert(sVal == val);
  static_assert(val != SignedBig{ static_cast<I8>(42) });

  constexpr auto unrVal = RestrictedBig<gSignedBig<0>, gSignedBig<100>>{ 50 };
  constexpr auto uVal   = UnsignedBig<8>{ 50 };
  static_assert(unrVal == uVal);
  static_assert(uVal == unrVal);
  static_assert(unrVal != UnsignedBig<8>{ 51 });
}

} // namespace

int
main(int /* argc */, char** /* argv */)
{
  try {
    testExhaustiveAgainstReference();
    testCrossModeEquality();
    testDisjointBoundsFold();
    testDegenerateRanges();
    testHeterogeneousOperands();
  } catch (std::exception const&) {
    std::exit(1); // NOLINT(*-mt-unsafe)
  }
}

// NOLINTEND(*-magic-numbers)
