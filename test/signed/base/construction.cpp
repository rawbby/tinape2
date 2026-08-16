#include <common.hpp>
#include <signed.hpp>
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
