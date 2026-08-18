#include <common.hpp>
#include <signed.hpp>
#include <unsigned.hpp>

#include <array>
#include <cstdlib>
#include <exception>

// NOLINTBEGIN(*-magic-numbers)

namespace {

using namespace big;
using PositiveSigned = SignedBig<Positive, 2>;
using NegativeSigned = SignedBig<Negative, 2>;
using RuntimeSigned  = SignedBig<Runtime, 2>;

// NOLINTBEGIN(*-identifier-naming,*-err58-cpp)
constexpr auto p3ct  = PositiveSigned{ 3 };
constexpr auto n3ct  = NegativeSigned{ 3 };
constexpr auto rp3ct = RuntimeSigned{ 3 };
constexpr auto rn3ct = RuntimeSigned{ -3 };
auto const     p3rt  = PositiveSigned{ 3 };
auto const     n3rt  = NegativeSigned{ 3 };
auto const     rp3rt = RuntimeSigned{ 3 };
auto const     rn3rt = RuntimeSigned{ -3 };
// NOLINTEND(*-identifier-naming,*-err58-cpp)

void
require(bool condition)
{
  if (!condition) {
    std::abort();
  }
}

void
testConstructionAbs()
{
  {
    constexpr auto p3ct2  = PositiveSigned{ p3ct };
    constexpr auto n3ct2  = NegativeSigned{ n3ct };
    constexpr auto rp3ct2 = RuntimeSigned{ p3ct };
    constexpr auto rp3ct3 = RuntimeSigned{ rp3ct };
    constexpr auto rn3ct2 = RuntimeSigned{ n3ct };
    constexpr auto rn3ct3 = RuntimeSigned{ rn3ct };

    static_assert(p3ct2.sign() == p3ct.sign());
    static_assert(p3ct2.abs() == p3ct.abs());

    static_assert(n3ct2.sign() == n3ct.sign());
    static_assert(n3ct2.abs() == n3ct.abs());

    static_assert(rp3ct2.sign() == p3ct.sign());
    static_assert(rp3ct2.abs() == p3ct.abs());

    static_assert(rp3ct3.sign() == rp3ct.sign());
    static_assert(rp3ct3.abs() == rp3ct.abs());

    static_assert(rn3ct2.sign() == n3ct.sign());
    static_assert(rn3ct2.abs() == n3ct.abs());

    static_assert(rn3ct3.sign() == rn3ct.sign());
    static_assert(rn3ct3.abs() == rn3ct.abs());

    static_assert(PositiveSigned{ p3ct }.sign() == p3ct.sign());
    static_assert(PositiveSigned{ p3ct }.abs() == p3ct.abs());

    static_assert(NegativeSigned{ n3ct }.sign() == n3ct.sign());
    static_assert(NegativeSigned{ n3ct }.abs() == n3ct.abs());

    static_assert(RuntimeSigned{ p3ct }.sign() == p3ct.sign());
    static_assert(RuntimeSigned{ p3ct }.abs() == p3ct.abs());

    static_assert(RuntimeSigned{ rp3ct }.sign() == rp3ct.sign());
    static_assert(RuntimeSigned{ rp3ct }.abs() == rp3ct.abs());

    static_assert(RuntimeSigned{ n3ct }.sign() == n3ct.sign());
    static_assert(RuntimeSigned{ n3ct }.abs() == n3ct.abs());

    static_assert(RuntimeSigned{ rn3ct }.sign() == rn3ct.sign());
    static_assert(RuntimeSigned{ rn3ct }.abs() == rn3ct.abs());
  }

  {
    static_assert(p3ct.sign());
    static_assert(!n3ct.sign());
    static_assert(rp3ct.sign());
    static_assert(!rn3ct.sign());

    static_assert(p3ct.abs() == 3);
    static_assert(n3ct.abs() == 3);
    static_assert(rp3ct.abs() == 3);
    static_assert(rn3ct.abs() == 3);

    static_assert(p3ct.abs() == p3ct.abs());
    static_assert(p3ct.abs() == n3ct.abs());
    static_assert(p3ct.abs() == rp3ct.abs());
    static_assert(p3ct.abs() == rn3ct.abs());

    static_assert(n3ct.abs() == p3ct.abs());
    static_assert(n3ct.abs() == n3ct.abs());
    static_assert(n3ct.abs() == rp3ct.abs());
    static_assert(n3ct.abs() == rn3ct.abs());

    static_assert(rp3ct.abs() == p3ct.abs());
    static_assert(rp3ct.abs() == n3ct.abs());
    static_assert(rp3ct.abs() == rp3ct.abs());
    static_assert(rp3ct.abs() == rn3ct.abs());

    static_assert(rn3ct.abs() == p3ct.abs());
    static_assert(rn3ct.abs() == n3ct.abs());
    static_assert(rn3ct.abs() == rp3ct.abs());
    static_assert(rn3ct.abs() == rn3ct.abs());

    static_assert(PositiveSigned{ 3 }.sign());
    static_assert(!NegativeSigned{ 3 }.sign());
    static_assert(RuntimeSigned{ 3 }.sign());
    static_assert(!RuntimeSigned{ -3 }.sign());

    static_assert(PositiveSigned{ 3 }.abs() == 3);
    static_assert(NegativeSigned{ 3 }.abs() == 3);
    static_assert(RuntimeSigned{ 3 }.abs() == 3);
    static_assert(RuntimeSigned{ -3 }.abs() == 3);
  }

  {
    auto const p3rt2  = PositiveSigned{ p3rt };
    auto const n3rt2  = NegativeSigned{ n3rt };
    auto const rp3rt2 = RuntimeSigned{ p3rt };
    auto const rp3rt3 = RuntimeSigned{ rp3rt };
    auto const rn3rt2 = RuntimeSigned{ n3rt };
    auto const rn3rt3 = RuntimeSigned{ rn3rt };

    require(p3rt2.sign() == p3rt.sign());
    require(p3rt2.abs() == p3rt.abs());

    require(n3rt2.sign() == n3rt.sign());
    require(n3rt2.abs() == n3rt.abs());

    require(rp3rt2.sign() == p3rt.sign());
    require(rp3rt2.abs() == p3rt.abs());

    require(rp3rt3.sign() == rp3rt.sign());
    require(rp3rt3.abs() == rp3rt.abs());

    require(rn3rt2.sign() == n3rt.sign());
    require(rn3rt2.abs() == n3rt.abs());

    require(rn3rt3.sign() == rn3rt.sign());
    require(rn3rt3.abs() == rn3rt.abs());

    require(PositiveSigned{ p3rt }.sign() == p3rt.sign());
    require(PositiveSigned{ p3rt }.abs() == p3rt.abs());

    require(NegativeSigned{ n3rt }.sign() == n3rt.sign());
    require(NegativeSigned{ n3rt }.abs() == n3rt.abs());

    require(RuntimeSigned{ p3rt }.sign() == p3rt.sign());
    require(RuntimeSigned{ p3rt }.abs() == p3rt.abs());

    require(RuntimeSigned{ rp3rt }.sign() == rp3rt.sign());
    require(RuntimeSigned{ rp3rt }.abs() == rp3rt.abs());

    require(RuntimeSigned{ n3rt }.sign() == n3rt.sign());
    require(RuntimeSigned{ n3rt }.abs() == n3rt.abs());

    require(RuntimeSigned{ rn3rt }.sign() == rn3rt.sign());
    require(RuntimeSigned{ rn3rt }.abs() == rn3rt.abs());
  }

  {
    require(p3rt.sign());
    require(!n3rt.sign());
    require(rp3rt.sign());
    require(!rn3rt.sign());

    require(p3rt.abs() == 3);
    require(n3rt.abs() == 3);
    require(rp3rt.abs() == 3);
    require(rn3rt.abs() == 3);

    require(p3rt.abs() == p3rt.abs());
    require(p3rt.abs() == n3rt.abs());
    require(p3rt.abs() == rp3rt.abs());
    require(p3rt.abs() == rn3rt.abs());

    require(n3rt.abs() == p3rt.abs());
    require(n3rt.abs() == n3rt.abs());
    require(n3rt.abs() == rp3rt.abs());
    require(n3rt.abs() == rn3rt.abs());

    require(rp3rt.abs() == p3rt.abs());
    require(rp3rt.abs() == n3rt.abs());
    require(rp3rt.abs() == rp3rt.abs());
    require(rp3rt.abs() == rn3rt.abs());

    require(rn3rt.abs() == p3rt.abs());
    require(rn3rt.abs() == n3rt.abs());
    require(rn3rt.abs() == rp3rt.abs());
    require(rn3rt.abs() == rn3rt.abs());

    require(PositiveSigned{ 3 }.sign());
    require(!NegativeSigned{ 3 }.sign());
    require(RuntimeSigned{ 3 }.sign());
    require(!RuntimeSigned{ -3 }.sign());

    require(PositiveSigned{ 3 }.abs() == 3);
    require(NegativeSigned{ 3 }.abs() == 3);
    require(RuntimeSigned{ 3 }.abs() == 3);
    require(RuntimeSigned{ -3 }.abs() == 3);
  }
}

void
testConstructionSign()
{
  static_assert(PositiveSigned{ 3 }.sign());
  static_assert(!NegativeSigned{ 3 }.sign());
  static_assert(RuntimeSigned{ 3 }.sign());
  static_assert(!RuntimeSigned{ -3 }.sign());
}

/// construction from unsigned integrals must store the exact absolute value
void
testConstructionFromUnsignedStoresValue()
{
  static_assert(SignedBig{ static_cast<U8>(8U) }.abs().mData == 8U);
  static_assert(SignedBig{ static_cast<U16>(16U) }.abs().mData == 16U);
  static_assert(SignedBig{ static_cast<U32>(32U) }.abs().mData == 32U);
  static_assert(SignedBig{ static_cast<U64>(64U) }.abs().mData == 64U);
  static_assert(SignedBig{ static_cast<Element>(64U) }.abs().mData == 64U);
  static_assert(SignedBig{ static_cast<Wide>(128U) }.abs().mData == 128U);
}

/// construction from signed integrals must split value into absolute value and sign
void
testConstructionFromSignedSplitsSign()
{
  constexpr auto positive = SignedBig{ static_cast<I8>(7) };
  static_assert(positive.abs().mData == 7U);
  static_assert(positive.sign());
  static_assert(positive == 7);

  constexpr auto negative = SignedBig{ static_cast<I8>(-7) };
  static_assert(negative.abs().mData == 7U);
  static_assert(!negative.sign());
  static_assert(negative == -7);
}

/// construction with an explicit sign flag must store both components
void
testConstructionWithExplicitSign()
{
  constexpr auto positive = SignedBig{ static_cast<U8>(7U), true };
  static_assert(positive.abs().mData == 7U);
  static_assert(positive.sign());

  constexpr auto negative = SignedBig{ static_cast<U8>(7U), false };
  static_assert(negative.abs().mData == 7U);
  static_assert(!negative.sign());
}

/// construction from element arrays must store elements little endian in the absolute value
void
testConstructionFromElements()
{
  constexpr auto value = SignedBig{ std::array{
    static_cast<Element>(1),
    static_cast<Element>(2),
    static_cast<Element>(3),
  } };
  static_assert(value.abs().mData[0] == static_cast<Element>(1));
  static_assert(value.abs().mData[1] == static_cast<Element>(2));
  static_assert(value.abs().mData[2] == static_cast<Element>(3));
  static_assert(value.abs().mData[0] == value.abs().element(0));
  static_assert(value.abs().mData[1] == value.abs().element(1));
  static_assert(value.abs().mData[2] == value.abs().element(2));
}

/// exhaustive construction round trip against the built in integer reference
void
testExhaustiveRoundTripAgainstReference()
{
  for (I32 value = -127; value <= 127; ++value) {
    auto const bigValue = SignedBig{ static_cast<I8>(value) };
    require(bigValue == value);
  }
}

} // namespace

int
main(int /* argc */, char** /* argv */)
{
  try {

    testConstructionFromUnsignedStoresValue();
    testConstructionFromSignedSplitsSign();
    testConstructionWithExplicitSign();
    testConstructionFromElements();
    testExhaustiveRoundTripAgainstReference();
  } catch (std::exception const&) {
    std::exit(1); // NOLINT(*-mt-unsafe)
  }
}

// NOLINTEND(*-magic-numbers)
