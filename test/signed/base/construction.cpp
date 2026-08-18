#include <common.hpp>
#include <signed.hpp>
#include <unsigned.hpp>

#include <array>
#include <cstdlib>
#include <exception>

// NOLINTBEGIN(*-magic-numbers)

namespace {

using namespace big;

using Positive8   = SignedBig<Positive, 8>;
using Negative8   = SignedBig<Negative, 8>;
using Runtime8    = SignedBig<Runtime, 8>;
using Positive4   = SignedBig<Positive, 4>;
using Runtime7    = SignedBig<Runtime, 7>;
using Positive64  = SignedBig<Positive, 64>;
using Runtime64   = SignedBig<Runtime, 64>;
using PositiveBig = SignedBig<Positive, 3 * gElementBits>;
using RuntimeBig  = SignedBig<Runtime, 3 * gElementBits>;

/// runtime assertion which aborts because the tests must fail loudly outside of constant evaluation
void
require(bool condition)
{
  if (!condition) {
    std::abort();
  }
}

/// hides a value from constant evaluation so that the runtime overloads are selected
/// a non constexpr function can never be part of a constant expression which makes the consteval overloads unavailable
template<typename Value>
[[nodiscard]] auto
opaque(Value value) -> Value
{
  return value;
}

/// type level constructibility which is satisfied by the runtime as well as by the consteval overloads
template<typename Target, typename... Args>
concept IsConstructible = requires(Args... args) { Target{ args... }; };

/// value level check whether an expression is a valid constant expression
/// the consteval only overloads reject invalid values by throwing during constant evaluation
template<typename Lambda, int = (Lambda{}(), 0)>
constexpr auto
isConstantEvaluable(Lambda /* lambda */) -> bool
{
  return true;
}

// NOLINTNEXTLINE(cert-dcl50-cpp)
constexpr auto
isConstantEvaluable(auto...) -> bool
{
  return false;
}

/// impl::IsSignedBigRhs describes every source which may be used at runtime
void
testRuntimeSourceConceptAcceptsCompatibleSources()
{
  // unsigned integrals keep their full width and take the sign from the target type
  static_assert(impl::IsSignedBigRhs<U8, Positive, 8>);
  static_assert(impl::IsSignedBigRhs<U8, Negative, 8>);
  static_assert(impl::IsSignedBigRhs<U8, Runtime, 8>);
  static_assert(impl::IsSignedBigRhs<U8, Positive, 64>);
  static_assert(impl::IsSignedBigRhs<Element, Positive, gElementBits>);
  static_assert(impl::IsSignedBigRhs<Wide, Positive, gWideBits>);

  // signed integrals lose their sign bit and require a runtime sign
  static_assert(impl::IsSignedBigRhs<I8, Runtime, 7>);
  static_assert(impl::IsSignedBigRhs<I64, Runtime, 63>);

  // unsigned big values take the sign from the target type
  static_assert(impl::IsSignedBigRhs<UnsignedBig<8>, Positive, 8>);
  static_assert(impl::IsSignedBigRhs<UnsignedBig<8>, Negative, 8>);
  static_assert(impl::IsSignedBigRhs<UnsignedBig<8>, Runtime, 8>);

  // signed big values keep their own sign and therefore need a matching or a runtime target sign
  static_assert(impl::IsSignedBigRhs<Positive8, Positive, 8>);
  static_assert(impl::IsSignedBigRhs<Negative8, Negative, 8>);
  static_assert(impl::IsSignedBigRhs<Runtime8, Runtime, 8>);
  static_assert(impl::IsSignedBigRhs<Positive8, Runtime, 8>);
  static_assert(impl::IsSignedBigRhs<Negative8, Runtime, 8>);

  // element and limb arrays behave like unsigned big values
  static_assert(impl::IsSignedBigRhs<Elements<3>, Positive, 3 * gElementBits>);
  if constexpr (!gLimbIsElement) {
    static_assert(impl::IsSignedBigRhs<Limbs<3>, Positive, 3 * gLimbBits>);
  }
}

/// impl::IsSignedBigRhs rejects every source which needs a compile time check
void
testRuntimeSourceConceptRejectsConstevalOnlySources()
{
  // signed integrals can not be stored in a compile time sign type because the sign is unknown
  static_assert(!impl::IsSignedBigRhs<I8, Positive, 7>);
  static_assert(!impl::IsSignedBigRhs<I8, Negative, 7>);

  // truncating sources need a compile time range check
  static_assert(!impl::IsSignedBigRhs<U16, Positive, 8>);
  static_assert(!impl::IsSignedBigRhs<UnsignedBig<16>, Positive, 8>);
  static_assert(!impl::IsSignedBigRhs<I16, Runtime, 7>);
  static_assert(!impl::IsSignedBigRhs<Elements<3>, Positive, 2 * gElementBits>);

  // a compile time sign type can not adopt a foreign sign
  static_assert(!impl::IsSignedBigRhs<Positive8, Negative, 8>);
  static_assert(!impl::IsSignedBigRhs<Negative8, Positive, 8>);
  static_assert(!impl::IsSignedBigRhs<Runtime8, Positive, 8>);
  static_assert(!impl::IsSignedBigRhs<Runtime8, Negative, 8>);

  // NOTE: current behaviour compares the sign type instead of the bit width for signed big sources
  // which lets a wide signed big source pass the runtime concept of a narrow target
  static_assert(impl::IsSignedBigRhs<Positive64, Positive, 8>);
  static_assert(impl::IsSignedBigRhs<Positive64, Positive, 1>);
  static_assert(!impl::IsSignedBigRhs<Positive64, Positive, 0>);
}

/// impl::IsSignedBigConstevalRhs covers exactly the sources which are rejected at runtime
void
testConstevalSourceConceptCoversRemainingSources()
{
  static_assert(!impl::IsSignedBigConstevalRhs<U8, Positive, 8>);
  static_assert(!impl::IsSignedBigConstevalRhs<I8, Runtime, 7>);
  static_assert(!impl::IsSignedBigConstevalRhs<Positive8, Runtime, 8>);

  static_assert(impl::IsSignedBigConstevalRhs<I8, Positive, 7>);
  static_assert(impl::IsSignedBigConstevalRhs<I8, Negative, 7>);
  static_assert(impl::IsSignedBigConstevalRhs<U16, Positive, 8>);
  static_assert(impl::IsSignedBigConstevalRhs<UnsignedBig<16>, Positive, 8>);
  static_assert(impl::IsSignedBigConstevalRhs<Elements<3>, Positive, 2 * gElementBits>);
  static_assert(impl::IsSignedBigConstevalRhs<Positive8, Negative, 8>);
  static_assert(impl::IsSignedBigConstevalRhs<Runtime8, Positive, 8>);

  // unrelated types are neither runtime nor consteval sources
  static_assert(!impl::IsSignedBigRhs<bool*, Positive, 8>);
  static_assert(!impl::IsSignedBigConstevalRhs<bool*, Positive, 8>);
  static_assert(!impl::IsSignedBigRhs<std::array<bool, 2>, Positive, 8>);
  static_assert(!impl::IsSignedBigConstevalRhs<std::array<bool, 2>, Positive, 8>);
}

/// every source of one of the two source concepts must be constructible
void
testConstructibilityMatchesSourceConcepts()
{
  static_assert(IsConstructible<Positive8, U8>);
  static_assert(IsConstructible<Positive8, I8>);
  static_assert(IsConstructible<Positive8, U16>);
  static_assert(IsConstructible<Positive8, UnsignedBig<8>>);
  static_assert(IsConstructible<Positive8, Positive8>);
  static_assert(IsConstructible<Runtime8, Positive8>);
  static_assert(IsConstructible<Runtime8, Negative8>);
  static_assert(IsConstructible<Runtime8, Runtime8>);
  static_assert(IsConstructible<Positive8, Runtime8>);

  static_assert(!IsConstructible<Positive8, bool*>);
  static_assert(!IsConstructible<Positive8, std::array<bool, 2>>);
}

/// the sign flag overload only exists for a runtime sign type
void
testSignFlagConstructorIsRuntimeSignOnly()
{
  static_assert(IsConstructible<Runtime8, U8, bool>);
  static_assert(IsConstructible<Runtime8, UnsignedBig<8>, bool>);
  static_assert(IsConstructible<RuntimeBig, Elements<3>, bool>);

  static_assert(!IsConstructible<Positive8, U8, bool>);
  static_assert(!IsConstructible<Negative8, U8, bool>);
}

/// the tag overload selects the sign type without changing the stored state
void
testTagConstructionSelectsSignType()
{
  static_assert(Is<decltype(SignedBig{ gTag<Positive>, static_cast<U8>(3U) }), Positive8>);
  static_assert(Is<decltype(SignedBig{ gTag<Negative>, static_cast<U8>(3U) }), Negative8>);
  static_assert(Is<decltype(SignedBig{ gTag<Runtime>, static_cast<U8>(3U) }), Runtime8>);

  static_assert(SignedBig{ gTag<Positive>, static_cast<U8>(3U) }.sign());
  static_assert(!SignedBig{ gTag<Negative>, static_cast<U8>(3U) }.sign());
  static_assert(SignedBig{ gTag<Runtime>, static_cast<U8>(3U) }.sign());

  static_assert(SignedBig{ gTag<Positive>, static_cast<U8>(3U) }.abs() == 3U);
  static_assert(SignedBig{ gTag<Negative>, static_cast<U8>(3U) }.abs() == 3U);
  static_assert(SignedBig{ gTag<Runtime>, static_cast<U8>(3U) }.abs() == 3U);
}

/// default construction must yield a zero magnitude and the sign of the type
void
testDefaultConstructionIsZero()
{
  static_assert(Positive8{}.abs().mData == 0U);
  static_assert(Positive8{}.sign());

  static_assert(Negative8{}.abs().mData == 0U);
  static_assert(!Negative8{}.sign());

  // a runtime sign type defaults to a negative sign because the flag is value initialised
  static_assert(Runtime8{}.abs().mData == 0U);
  static_assert(!Runtime8{}.sign());

  require(Positive8{}.abs().mData == 0U);
  require(Runtime8{}.abs().mData == 0U);
  require(!Runtime8{}.sign());
}

/// construction from unsigned integrals must store the exact magnitude and take the sign from the type
void
testConstructionFromUnsignedIntegralStoresMagnitude()
{
  static_assert(SignedBig{ static_cast<U8>(8U) }.abs().mData == 8U);
  static_assert(SignedBig{ static_cast<U16>(16U) }.abs().mData == 16U);
  static_assert(SignedBig{ static_cast<U32>(32U) }.abs().mData == 32U);
  static_assert(SignedBig{ static_cast<U64>(64U) }.abs().mData == 64U);
  static_assert(SignedBig{ static_cast<Element>(64U) }.abs().mData == 64U);
  static_assert(SignedBig{ static_cast<Wide>(128U) }.abs().mData == 128U);

  static_assert(Positive8{ static_cast<U8>(3U) }.sign());
  static_assert(!Negative8{ static_cast<U8>(3U) }.sign());
  static_assert(Runtime8{ static_cast<U8>(3U) }.sign());

  // the same holds for values which are hidden from constant evaluation
  require(Positive8{ opaque(static_cast<U8>(3U)) }.abs() == 3U);
  require(Positive8{ opaque(static_cast<U8>(3U)) }.sign());
  require(Negative8{ opaque(static_cast<U8>(3U)) }.abs() == 3U);
  require(!Negative8{ opaque(static_cast<U8>(3U)) }.sign());
  require(Runtime8{ opaque(static_cast<U8>(3U)) }.abs() == 3U);
  require(Runtime8{ opaque(static_cast<U8>(3U)) }.sign());
}

/// construction from signed integrals must split the value into magnitude and sign
void
testConstructionFromSignedIntegralSplitsSign()
{
  constexpr auto positive = SignedBig{ static_cast<I8>(7) };
  static_assert(Is<decltype(positive), Runtime7 const>);
  static_assert(positive.abs().mData == 7U);
  static_assert(positive.sign());
  static_assert(positive == 7);

  constexpr auto negative = SignedBig{ static_cast<I8>(-7) };
  static_assert(negative.abs().mData == 7U);
  static_assert(!negative.sign());
  static_assert(negative == -7);

  static_assert(SignedBig{ static_cast<I8>(0) }.abs().mData == 0U);
  static_assert(SignedBig{ static_cast<I8>(0) }.sign());

  // the most negative value of the source type must keep its magnitude
  static_assert(SignedBig{ static_cast<I8>(-127) }.abs().mData == 127U);
  static_assert(!SignedBig{ static_cast<I8>(-127) }.sign());

  require(Runtime7{ opaque(static_cast<I8>(7)) }.abs() == 7U);
  require(Runtime7{ opaque(static_cast<I8>(7)) }.sign());
  require(Runtime7{ opaque(static_cast<I8>(-7)) }.abs() == 7U);
  require(!Runtime7{ opaque(static_cast<I8>(-7)) }.sign());
  require(Runtime7{ opaque(static_cast<I8>(0)) }.sign());
}

/// construction with an explicit sign flag must store both components as given
void
testConstructionWithExplicitSignStoresBoth()
{
  constexpr auto positive = SignedBig{ static_cast<U8>(7U), true };
  static_assert(Is<decltype(positive), Runtime8 const>);
  static_assert(positive.abs().mData == 7U);
  static_assert(positive.sign());

  constexpr auto negative = SignedBig{ static_cast<U8>(7U), false };
  static_assert(negative.abs().mData == 7U);
  static_assert(!negative.sign());

  // a zero magnitude keeps the requested sign
  static_assert(!SignedBig{ static_cast<U8>(0U), false }.sign());
  static_assert(SignedBig{ static_cast<U8>(0U), true }.sign());

  require(Runtime8{ opaque(static_cast<U8>(7U)), opaque(true) }.abs() == 7U);
  require(Runtime8{ opaque(static_cast<U8>(7U)), opaque(true) }.sign());
  require(Runtime8{ opaque(static_cast<U8>(7U)), opaque(false) }.abs() == 7U);
  require(!Runtime8{ opaque(static_cast<U8>(7U)), opaque(false) }.sign());
}

/// construction from unsigned big must copy the magnitude and take the sign from the target type
void
testConstructionFromUnsignedBigTakesSignFromType()
{
  constexpr auto magnitude = UnsignedBig{ static_cast<U8>(3U) };

  static_assert(Positive8{ magnitude }.abs() == magnitude);
  static_assert(Positive8{ magnitude }.sign());

  static_assert(Negative8{ magnitude }.abs() == magnitude);
  static_assert(!Negative8{ magnitude }.sign());

  static_assert(Runtime8{ magnitude }.abs() == magnitude);
  static_assert(Runtime8{ magnitude }.sign());

  static_assert(Runtime8{ magnitude, false }.abs() == magnitude);
  static_assert(!Runtime8{ magnitude, false }.sign());

  // deduction from unsigned big defaults to a positive sign type
  static_assert(Is<decltype(SignedBig{ magnitude }), Positive8>);

  require(Positive8{ opaque(magnitude) }.abs() == magnitude);
  require(Positive8{ opaque(magnitude) }.sign());
  require(!Negative8{ opaque(magnitude) }.sign());
  require(!Runtime8{ opaque(magnitude), opaque(false) }.sign());
}

/// construction from another signed big must preserve the sign of the source value
void
testConstructionFromSignedBigPreservesSourceSign()
{
  constexpr auto positive        = Positive8{ static_cast<U8>(3U) };
  constexpr auto negative        = Negative8{ static_cast<U8>(3U) };
  constexpr auto runtimePositive = Runtime8{ static_cast<U8>(3U), true };
  constexpr auto runtimeNegative = Runtime8{ static_cast<U8>(3U), false };

  static_assert(Positive8{ positive }.sign() == positive.sign());
  static_assert(Positive8{ positive }.abs() == positive.abs());

  static_assert(Negative8{ negative }.sign() == negative.sign());
  static_assert(Negative8{ negative }.abs() == negative.abs());

  static_assert(Runtime8{ positive }.sign() == positive.sign());
  static_assert(Runtime8{ positive }.abs() == positive.abs());

  static_assert(Runtime8{ negative }.sign() == negative.sign());
  static_assert(Runtime8{ negative }.abs() == negative.abs());

  static_assert(Runtime8{ runtimePositive }.sign() == runtimePositive.sign());
  static_assert(Runtime8{ runtimePositive }.abs() == runtimePositive.abs());

  static_assert(Runtime8{ runtimeNegative }.sign() == runtimeNegative.sign());
  static_assert(Runtime8{ runtimeNegative }.abs() == runtimeNegative.abs());

  // widening keeps both components
  static_assert(Runtime64{ negative }.sign() == negative.sign());
  static_assert(Runtime64{ negative }.abs() == negative.abs());
  static_assert(Positive64{ positive }.abs() == positive.abs());

  // the magnitude is independent of the sign of the source
  static_assert(positive.abs() == negative.abs());
  static_assert(positive.abs() == runtimePositive.abs());
  static_assert(positive.abs() == runtimeNegative.abs());

  require(Runtime8{ opaque(positive) }.sign());
  require(Runtime8{ opaque(positive) }.abs() == 3U);
  require(!Runtime8{ opaque(negative) }.sign());
  require(Runtime8{ opaque(negative) }.abs() == 3U);
  require(!Runtime8{ opaque(runtimeNegative) }.sign());
  require(Runtime8{ opaque(runtimeNegative) }.abs() == 3U);
  require(Runtime64{ opaque(runtimeNegative) }.abs() == 3U);
}

/// construction from element arrays must store the elements little endian in the magnitude
void
testConstructionFromElementsStoresLittleEndian()
{
  constexpr auto value = SignedBig{ Elements<3>{ 1, 2, 3 } };
  static_assert(Is<decltype(value), PositiveBig const>);

  static_assert(value.abs().mData[0] == static_cast<Element>(1));
  static_assert(value.abs().mData[1] == static_cast<Element>(2));
  static_assert(value.abs().mData[2] == static_cast<Element>(3));
  static_assert(value.abs().element(0) == value.abs().mData[0]);
  static_assert(value.abs().element(1) == value.abs().mData[1]);
  static_assert(value.abs().element(2) == value.abs().mData[2]);
  static_assert(value.sign());

  static_assert(!SignedBig{ Elements<3>{ 1, 2, 3 }, false }.sign());
  static_assert(SignedBig{ Elements<3>{ 1, 2, 3 }, false }.abs() == value.abs());

  auto const runtimeValue = PositiveBig{ opaque(Elements<3>{ 1, 2, 3 }) };
  require(runtimeValue.abs().element(0) == static_cast<Element>(1));
  require(runtimeValue.abs().element(2) == static_cast<Element>(3));
  require(runtimeValue.sign());
}

/// construction from limb arrays must pack two limbs into one element
void
testConstructionFromLimbsStoresLittleEndian()
{
  if constexpr (!gLimbIsElement) {
    constexpr auto value = SignedBig{ Limbs<3>{ 1, 2, 3 } };
    static_assert(value.abs().limb(0) == static_cast<Limb>(1));
    static_assert(value.abs().limb(1) == static_cast<Limb>(2));
    static_assert(value.abs().limb(2) == static_cast<Limb>(3));
    static_assert(value.sign());

    auto const runtimeValue = SignedBig<Positive, 3 * gLimbBits>{ opaque(Limbs<3>{ 1, 2, 3 }) };
    require(runtimeValue.abs().limb(0) == static_cast<Limb>(1));
    require(runtimeValue.abs().limb(2) == static_cast<Limb>(3));
  }
}

/// the consteval only overloads must reject every value which does not fit the target
void
testConstevalConstructionRejectsInvalidValues()
{
  // a positive sign type only accepts non negative values
  static_assert(isConstantEvaluable([] { return Positive8{ static_cast<I8>(3) }; }));
  static_assert(!isConstantEvaluable([] { return Positive8{ static_cast<I8>(-3) }; }));

  // a negative sign type also only accepts non negative values because the sign comes from the type
  static_assert(isConstantEvaluable([] { return Negative8{ static_cast<I8>(3) }; }));
  static_assert(!isConstantEvaluable([] { return Negative8{ static_cast<I8>(-3) }; }));
  static_assert(!Negative8{ static_cast<I8>(3) }.sign());

  // a runtime sign type accepts both signs
  static_assert(isConstantEvaluable([] { return Runtime7{ static_cast<I8>(-3) }; }));

  // truncating magnitudes are rejected
  static_assert(isConstantEvaluable([] { return Positive4{ static_cast<U8>(15U) }; }));
  static_assert(!isConstantEvaluable([] { return Positive4{ static_cast<U8>(16U) }; }));
  static_assert(!isConstantEvaluable([] { return Positive4{ static_cast<U16>(300U) }; }));
  static_assert(!isConstantEvaluable([] { return Positive8{ UnsignedBig{ static_cast<U16>(300U) } }; }));
  static_assert(!isConstantEvaluable([] { return Positive8{ Elements<2>{ 1, 2 } }; }));

  // NOTE: current behaviour lets a wide signed big source use the runtime overload
  // which still checks the magnitude during constant evaluation
  static_assert(isConstantEvaluable([] { return Positive8{ Positive64{ static_cast<U8>(3U) } }; }));
  static_assert(!isConstantEvaluable([] { return Positive8{ Positive64{ static_cast<U16>(300U) } }; }));

  // a foreign compile time sign can not be adopted
  static_assert(!isConstantEvaluable([] { return Positive8{ Negative8{ static_cast<U8>(3U) } }; }));
  static_assert(!isConstantEvaluable([] { return Negative8{ Positive8{ static_cast<U8>(3U) } }; }));

  // a runtime sign is only accepted if it matches the target sign type
  static_assert(isConstantEvaluable([] { return Positive8{ Runtime8{ static_cast<U8>(3U), true } }; }));
  static_assert(!isConstantEvaluable([] { return Positive8{ Runtime8{ static_cast<U8>(3U), false } }; }));
  static_assert(isConstantEvaluable([] { return Negative8{ Runtime8{ static_cast<U8>(3U), false } }; }));
  static_assert(!isConstantEvaluable([] { return Negative8{ Runtime8{ static_cast<U8>(3U), true } }; }));
}

/// assignment must behave exactly like construction
void
testAssignmentBehavesLikeConstruction()
{
  auto value = Runtime8{ opaque(static_cast<U8>(1U)), opaque(true) };

  value = opaque(static_cast<I8>(-5));
  require(value.abs() == 5U);
  require(!value.sign());

  value = opaque(static_cast<U8>(6U));
  require(value.abs() == 6U);
  require(value.sign());

  value = opaque(Negative8{ static_cast<U8>(7U) });
  require(value.abs() == 7U);
  require(!value.sign());

  value = opaque(Runtime8{ static_cast<U8>(8U), true });
  require(value.abs() == 8U);
  require(value.sign());
}

/// exhaustive construction round trip against the built in integer reference
void
testExhaustiveRoundTripAgainstReference()
{
  for (I32 value = -127; value <= 127; ++value) {
    auto const bigValue = SignedBig{ static_cast<I8>(value) };
    require(bigValue == value);
    require(bigValue.sign() == (value >= 0));
    require(bigValue.abs() == static_cast<U32>(value < 0 ? -value : value));
  }
}

} // namespace

int
main(int /* argc */, char** /* argv */)
{
  try {
    testRuntimeSourceConceptAcceptsCompatibleSources();
    testRuntimeSourceConceptRejectsConstevalOnlySources();
    testConstevalSourceConceptCoversRemainingSources();
    testConstructibilityMatchesSourceConcepts();
    testSignFlagConstructorIsRuntimeSignOnly();
    testTagConstructionSelectsSignType();
    testDefaultConstructionIsZero();
    testConstructionFromUnsignedIntegralStoresMagnitude();
    testConstructionFromSignedIntegralSplitsSign();
    testConstructionWithExplicitSignStoresBoth();
    testConstructionFromUnsignedBigTakesSignFromType();
    testConstructionFromSignedBigPreservesSourceSign();
    testConstructionFromElementsStoresLittleEndian();
    testConstructionFromLimbsStoresLittleEndian();
    testConstevalConstructionRejectsInvalidValues();
    testAssignmentBehavesLikeConstruction();
    testExhaustiveRoundTripAgainstReference();
  } catch (std::exception const&) {
    std::exit(1); // NOLINT(*-mt-unsafe)
  }
}

// NOLINTEND(*-magic-numbers)
