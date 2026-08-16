#include <common.hpp>
#include <unsigned.hpp>

#include <array>
#include <cstdlib>
#include <exception>

// NOLINTBEGIN(*-magic-numbers)

namespace {

using namespace big;

/// construction from integrals must store the exact value
void
testConstructionStoresValue()
{
  static_assert(UnsignedBig{ static_cast<U8>(8U) }.mData == 8U);
  static_assert(UnsignedBig{ static_cast<U16>(16U) }.mData == 16U);
  static_assert(UnsignedBig{ static_cast<U32>(32U) }.mData == 32U);
  static_assert(UnsignedBig{ static_cast<U64>(64U) }.mData == 64U);
  static_assert(UnsignedBig{ static_cast<Element>(64U) }.mData == 64U);
  static_assert(UnsignedBig{ static_cast<Wide>(128U) }.mData == 128U);
  static_assert(UnsignedBig{ static_cast<I8>(7) }.mData == 7U);
  static_assert(UnsignedBig{ static_cast<I64>(63) }.mData == 63U);
}

/// default construction must yield zero
void
testDefaultConstructionIsZero()
{
  constexpr auto value = UnsignedBig<8>{};
  static_assert(value.mData == 0U);
}

/// construction from element arrays must store elements little endian
void
testConstructionFromElements()
{
  constexpr auto value = UnsignedBig{ std::array{
    static_cast<Element>(1),
    static_cast<Element>(2),
    static_cast<Element>(3),
  } };
  static_assert(value.mData[0] == static_cast<Element>(1));
  static_assert(value.mData[1] == static_cast<Element>(2));
  static_assert(value.mData[2] == static_cast<Element>(3));
}

/// copy construction and copy assignment must preserve the value
void
testCopySemantics()
{
  constexpr auto original = UnsignedBig{ static_cast<U8>(42U) };
  constexpr auto copy     = original;
  static_assert(copy == original);
  static_assert(copy.mData == 42U);
}

} // namespace

int
main(int /* argc */, char** /* argv */)
{
  try {
    testConstructionStoresValue();
    testDefaultConstructionIsZero();
    testConstructionFromElements();
    testCopySemantics();
  } catch (std::exception const&) {
    std::exit(1); // NOLINT(*-mt-unsafe)
  }
}

// NOLINTEND(*-magic-numbers)
