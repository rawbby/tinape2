#include <common.hpp>
#include <restricted.hpp>
#include <signed.hpp>
#include <unsigned.hpp>

#include <array>
#include <cstdlib>
#include <exception>
#include <type_traits>

// NOLINTBEGIN(*-magic-numbers)

namespace {

using namespace big;

/// runtime assertion which aborts because the tests must fail loudly outside of constant evaluation
void
require(bool condition)
{
  if (!condition) {
    std::abort();
  }
}

/// hides a value from constant evaluation so that the runtime overloads are selected
template<typename Value>
[[nodiscard]] auto
opaque(Value value) -> Value
{
  return value;
}

/// type level constructibility
template<typename Target, typename... Args>
concept IsConstructible = requires(Args... args) { Target{ args... }; };

/// value level check whether an expression is a valid constant expression
template<typename Lambda, int = (Lambda{}(), 0)>
constexpr bool
isConstantEvaluable(Lambda /* lambda */)
{
  return true;
}

// NOLINTNEXTLINE(cert-dcl50-cpp)
constexpr bool
isConstantEvaluable(auto... /*unused*/)
{
  return false;
}

void
testSourceConcepts()
{
  // Target: [0, 255]
  constexpr auto lb0   = gSignedBig<0>;
  constexpr auto ub255 = gSignedBig<255>;

  // U8 has range [0, 255], fits completely
  static_assert(impl::IsRestrictedBigRhs<U8, lb0, ub255>);
  static_assert(!impl::IsRestrictedBigConstevalRhs<U8, lb0, ub255>);

  // Target: [0, 10]
  constexpr auto ub10 = gSignedBig<10>;
  // U8 does not fit completely, requires consteval check
  static_assert(!impl::IsRestrictedBigRhs<U8, lb0, ub10>);
  static_assert(impl::IsRestrictedBigConstevalRhs<U8, lb0, ub10>);

  // I8 has range [-127, 127]
  constexpr auto lbNeg127 = gSignedBig<-127>;
  constexpr auto ub127    = gSignedBig<127>;
  static_assert(impl::IsRestrictedBigRhs<I8, lbNeg127, ub127>);
  static_assert(!impl::IsRestrictedBigRhs<I8, lb0, ub127>);
  static_assert(impl::IsRestrictedBigConstevalRhs<I8, lb0, ub127>);

  // RestrictedBig contained in target
  using SubR    = RestrictedBig<gSignedBig<2>, gSignedBig<8>>;
  using TargetR = RestrictedBig<gSignedBig<0>, gSignedBig<10>>;
  static_assert(impl::IsRestrictedBigRhs<SubR, TargetR::gLb, TargetR::gUb>);

  // RestrictedBig not contained in target
  using WideR = RestrictedBig<gSignedBig<-1>, gSignedBig<12>>;
  static_assert(!impl::IsRestrictedBigRhs<WideR, TargetR::gLb, TargetR::gUb>);
  static_assert(impl::IsRestrictedBigConstevalRhs<WideR, TargetR::gLb, TargetR::gUb>);

  // Unrelated types
  static_assert(!impl::IsRestrictedBigRhs<bool*, lb0, ub255>);
  static_assert(!impl::IsRestrictedBigConstevalRhs<bool*, lb0, ub255>);
  static_assert(!impl::IsRestrictedBigRhs<std::array<bool, 2>, lb0, ub255>);
  static_assert(!impl::IsRestrictedBigConstevalRhs<std::array<bool, 2>, lb0, ub255>);
}

void
testDefaultConstruction()
{
  using R            = RestrictedBig<gSignedBig<-5>, gSignedBig<10>>;
  constexpr auto val = R{};
  static_assert(val.absolute() == 0);

  require(R{}.absolute() == 0);
  require(!R{}.sign());

  // degenerate [0, 0]
  using ZeroR            = RestrictedBig<gSignedBig<0>, gSignedBig<0>>;
  constexpr auto zeroVal = ZeroR{};
  static_assert(zeroVal.absolute() == 0);
  static_assert(zeroVal.sign());
  require(ZeroR{}.absolute() == 0);
  require(ZeroR{}.sign());
}

void
testNaiveModeConstruction()
{
  // Positive range: [5, 15]
  using PosR = RestrictedBig<gSignedBig<5>, gSignedBig<15>>;
  static_assert(PosR::gMode == RestrictedMode::Naive);

  constexpr auto posVal = PosR{ 10 };
  static_assert(posVal.absolute() == 10);
  static_assert(posVal.sign());

  using SubPosR         = RestrictedBig<gSignedBig<6>, gSignedBig<12>>;
  auto const runtimePos = PosR{ opaque(SubPosR{ 7 }) };
  require(runtimePos.absolute() == 7);
  require(runtimePos.sign());

  // Negative range: [-15, -5]
  using NegR = RestrictedBig<gSignedBig<-15>, gSignedBig<-5>>;
  static_assert(NegR::gMode == RestrictedMode::Naive);

  constexpr auto negVal = NegR{ -10 };
  static_assert(negVal.absolute() == -10);
  static_assert(!negVal.sign());

  using SubNegR         = RestrictedBig<gSignedBig<-14>, gSignedBig<-6>>;
  auto const runtimeNeg = NegR{ opaque(SubNegR{ -12 }) };
  require(runtimeNeg.absolute() == -12);
  require(!runtimeNeg.sign());

  // Straddling range: [-10, 10]
  using StraddleR = RestrictedBig<gSignedBig<-10>, gSignedBig<10>>;
  static_assert(StraddleR::gMode == RestrictedMode::Naive);

  constexpr auto straddleNeg = StraddleR{ -7 };
  static_assert(straddleNeg.absolute() == -7);
  static_assert(!straddleNeg.sign());

  constexpr auto straddlePos = StraddleR{ 7 };
  static_assert(straddlePos.absolute() == 7);
  static_assert(straddlePos.sign());

  constexpr auto straddleZero = StraddleR{ 0 };
  static_assert(straddleZero.absolute() == 0);

  using SubStraddleR            = RestrictedBig<gSignedBig<-5>, gSignedBig<5>>;
  auto const runtimeStraddleNeg = StraddleR{ opaque(SubStraddleR{ -4 }) };
  require(runtimeStraddleNeg.absolute() == -4);
  require(!runtimeStraddleNeg.sign());

  auto const runtimeStraddlePos = StraddleR{ opaque(SubStraddleR{ 4 }) };
  require(runtimeStraddlePos.absolute() == 4);
  require(runtimeStraddlePos.sign());

  // Runtime construction from native types with contained range
  using ByteR            = RestrictedBig<gSignedBig<0>, gSignedBig<255>>;
  auto const runtimeByte = ByteR{ opaque(static_cast<U8>(42U)) };
  require(runtimeByte.absolute() == 42);

  using SignedByteR            = RestrictedBig<gSignedBig<-127>, gSignedBig<127>>;
  auto const runtimeSignedByte = SignedByteR{ opaque(static_cast<I8>(-42)) };
  require(runtimeSignedByte.absolute() == -42);
}

void
testExactShiftModeConstruction()
{
  // Multi-element bounds with small span (positive)
  constexpr auto posBase = SignedBig<Positive, 3 * gElementBits>{ Elements<3>{ 10, 0, 1 } };
  constexpr auto posLb   = gSignedBig<posBase>;
  constexpr auto posUb   = gSignedBig<posBase + 10>;

  using ExactPosR = RestrictedBig<posLb, posUb>;
  static_assert(ExactPosR::gMode == RestrictedMode::ExactShift);

  constexpr auto exactPos = ExactPosR{ posBase + 4 };
  static_assert(exactPos.absolute() == posBase + 4);
  static_assert(exactPos.sign());

  using SubExactPosR         = RestrictedBig<gSignedBig<posBase + 2>, gSignedBig<posBase + 8>>;
  auto const runtimeExactPos = ExactPosR{ opaque(SubExactPosR{ posBase + 7 }) };
  require(runtimeExactPos.absolute() == posBase + 7);
  require(runtimeExactPos.sign());

  // Multi-element bounds with small span (negative)
  constexpr auto negBase = SignedBig<Negative, 3 * gElementBits>{ Elements<3>{ 20, 0, 1 } };
  constexpr auto negLb   = gSignedBig<negBase>;
  constexpr auto negUb   = gSignedBig<negBase + 10>;

  using ExactNegR = RestrictedBig<negLb, negUb>;
  static_assert(ExactNegR::gMode == RestrictedMode::ExactShift);

  constexpr auto exactNeg = ExactNegR{ negBase + 3 };
  static_assert(exactNeg.absolute() == negBase + 3);
  static_assert(!exactNeg.sign());

  using SubExactNegR         = RestrictedBig<gSignedBig<negBase + 2>, gSignedBig<negBase + 8>>;
  auto const runtimeExactNeg = ExactNegR{ opaque(SubExactNegR{ negBase + 8 }) };
  require(runtimeExactNeg.absolute() == negBase + 8);
  require(!runtimeExactNeg.sign());

  // Degenerate multi-element bound: lb == ub
  using ExactDegenR = RestrictedBig<posLb, posLb>;
  static_assert(ExactDegenR::gMode == RestrictedMode::ExactShift);
  constexpr auto degenVal = ExactDegenR{ posLb };
  static_assert(degenVal.absolute() == posLb);
  require(ExactDegenR{ opaque(ExactDegenR{ posLb }) }.absolute() == posLb);
}

void
testElementShiftModeConstruction()
{
  // Multi-element bounds with span exceeding gWideBits (positive)
  constexpr auto posBase   = SignedBig<Positive, 4 * gElementBits>{ Elements<4>{ 15, 0, 0, 1 } };
  constexpr auto largeSpan = SignedBig<Positive, 3 * gElementBits>{ Elements<3>{ 1, 1, 1 } };
  constexpr auto elemPosLb = gSignedBig<posBase>;
  constexpr auto elemPosUb = gSignedBig<posBase + largeSpan>;

  using ElemPosR = RestrictedBig<elemPosLb, elemPosUb>;
  static_assert(ElemPosR::gMode == RestrictedMode::ElementShift);

  constexpr auto elemPos = ElemPosR{ posBase + 100 };
  static_assert(elemPos.absolute() == posBase + 100);
  static_assert(elemPos.sign());

  using SubElemPosR         = RestrictedBig<gSignedBig<posBase + 50>, gSignedBig<posBase + 250>>;
  auto const runtimeElemPos = ElemPosR{ opaque(SubElemPosR{ posBase + 200 }) };
  require(runtimeElemPos.absolute() == posBase + 200);
  require(runtimeElemPos.sign());

  // Large bounds straddling zero
  constexpr auto negBase   = SignedBig<Negative, 3 * gElementBits>{ Elements<3>{ 10, 0, 1 } };
  constexpr auto elemNegLb = gSignedBig<negBase>;
  constexpr auto elemNegUb = gSignedBig<posBase>;

  using ElemStraddleR = RestrictedBig<elemNegLb, elemNegUb>;
  static_assert(ElemStraddleR::gMode == RestrictedMode::ElementShift);

  constexpr auto elemStraddleNeg = ElemStraddleR{ negBase + 50 };
  static_assert(elemStraddleNeg.absolute() == negBase + 50);
  static_assert(!elemStraddleNeg.sign());

  constexpr auto elemStraddlePos = ElemStraddleR{ posBase - 50 };
  static_assert(elemStraddlePos.absolute() == posBase - 50);
  static_assert(elemStraddlePos.sign());

  constexpr auto elemStraddleZero = ElemStraddleR{ 0 };
  static_assert(elemStraddleZero.absolute() == 0);

  using SubElemStraddleR        = RestrictedBig<gSignedBig<negBase + 10>, gSignedBig<posBase - 10>>;
  auto const runtimeStraddleNeg = ElemStraddleR{ opaque(SubElemStraddleR{ negBase + 20 }) };
  require(runtimeStraddleNeg.absolute() == negBase + 20);
  require(!runtimeStraddleNeg.sign());

  auto const runtimeStraddlePos = ElemStraddleR{ opaque(SubElemStraddleR{ posBase - 30 }) };
  require(runtimeStraddlePos.absolute() == posBase - 30);
  require(runtimeStraddlePos.sign());
}

void
testConstevalRejectsInvalidValues()
{
  using R = RestrictedBig<gSignedBig<5>, gSignedBig<15>>;

  static_assert(isConstantEvaluable([] { return R{ 5 }; }));
  static_assert(isConstantEvaluable([] { return R{ 10 }; }));
  static_assert(isConstantEvaluable([] { return R{ 15 }; }));

  // Below lb
  static_assert(!isConstantEvaluable([] { return R{ 4 }; }));
  static_assert(!isConstantEvaluable([] { return R{ 0 }; }));
  static_assert(!isConstantEvaluable([] { return R{ -1 }; }));

  // Above ub
  static_assert(!isConstantEvaluable([] { return R{ 16 }; }));
  static_assert(!isConstantEvaluable([] { return R{ 100 }; }));

  // Narrowing from another RestrictedBig
  using WideR = RestrictedBig<gSignedBig<0>, gSignedBig<20>>;
  static_assert(isConstantEvaluable([] { return R{ WideR{ 7 } }; }));
  static_assert(!isConstantEvaluable([] { return R{ WideR{ 2 } }; }));
  static_assert(!isConstantEvaluable([] { return R{ WideR{ 18 } }; }));
}

void
testMinAndMax()
{
  // Naive mode
  using R1 = RestrictedBig<gSignedBig<-5>, gSignedBig<10>>;
  static_assert(R1::min().absolute() == -5);
  static_assert(R1::max().absolute() == 10);
  require(R1::min().absolute() == -5);
  require(R1::max().absolute() == 10);

  // ExactShift mode
  constexpr auto posBase = SignedBig<Positive, 3 * gElementBits>{ Elements<3>{ 10, 0, 1 } };
  constexpr auto posLb   = gSignedBig<posBase>;
  constexpr auto posUb   = gSignedBig<posBase + 10>;
  using R2               = RestrictedBig<posLb, posUb>;
  static_assert(R2::min().absolute() == posLb);
  static_assert(R2::max().absolute() == posUb);
  require(R2::min().absolute() == posLb);
  require(R2::max().absolute() == posUb);

  // ElementShift mode
  constexpr auto largeSpan = SignedBig<Positive, 3 * gElementBits>{ Elements<3>{ 1, 1, 1 } };
  using R3                 = RestrictedBig<posLb, gSignedBig<posBase + largeSpan>>;
  static_assert(R3::min().absolute() == posLb);
  static_assert(R3::max().absolute() == posBase + largeSpan);
  require(R3::min().absolute() == posLb);
  require(R3::max().absolute() == posBase + largeSpan);
}

void
testAssignment()
{
  using R  = RestrictedBig<gSignedBig<-10>, gSignedBig<10>>;
  auto val = R{};

  using SubR1 = RestrictedBig<gSignedBig<0>, gSignedBig<5>>;
  val         = opaque(SubR1{ 5 });
  require(val.absolute() == 5);
  require(val.sign());

  using SubR2 = RestrictedBig<gSignedBig<-8>, gSignedBig<0>>;
  val         = opaque(SubR2{ -8 });
  require(val.absolute() == -8);
  require(!val.sign());
}

void
testConstantsAndAlias()
{
  // gRestrictedBig
  constexpr auto zeroConst = gRestrictedBig<0>;
  static_assert(zeroConst.absolute() == 0);
  static_assert(zeroConst.sign());
  static_assert(decltype(zeroConst)::gLb == 0);
  static_assert(decltype(zeroConst)::gUb == 0);

  constexpr auto posConst = gRestrictedBig<42>;
  static_assert(posConst.absolute() == 42);
  static_assert(posConst.sign());
  static_assert(decltype(posConst)::gLb == 42);
  static_assert(decltype(posConst)::gUb == 42);

  constexpr auto negConst = gRestrictedBig<-17>;
  static_assert(negConst.absolute() == -17);
  static_assert(!negConst.sign());
  static_assert(decltype(negConst)::gLb == -17);
  static_assert(decltype(negConst)::gUb == -17);

  // Restricted alias
  using AliasR  = Restricted<-5, 10>;
  using DirectR = RestrictedBig<gSignedBig<-5>, gSignedBig<10>>;
  static_assert(std::is_same_v<AliasR, DirectR>);
}

void
testExhaustiveRoundTrip()
{
  using R = Restricted<-127, 127>;
  for (I32 val = -127; val <= 127; ++val) {
    auto const restrictedVal = R{ opaque(static_cast<I8>(val)) };
    require(restrictedVal.absolute() == val);
    require(restrictedVal.sign() == (val >= 0));
  }
}

} // namespace

int
main(int /* argc */, char** /* argv */)
{
  try {
    testSourceConcepts();
    testDefaultConstruction();
    testNaiveModeConstruction();
    testExactShiftModeConstruction();
    testElementShiftModeConstruction();
    testConstevalRejectsInvalidValues();
    testMinAndMax();
    testAssignment();
    testConstantsAndAlias();
    testExhaustiveRoundTrip();
  } catch (std::exception const&) {
    std::exit(1); // NOLINT(*-mt-unsafe)
  }
}

// NOLINTEND(*-magic-numbers)
