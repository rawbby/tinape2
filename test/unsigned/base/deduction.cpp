#include <common.hpp>
#include <unsigned.hpp>

#include <array>
#include <cstdlib>
#include <exception>

// NOLINTBEGIN(*-magic-numbers)

namespace {

using namespace big;

/// deduction from unsigned integrals must use the full bit width of the source type
void
testDeductionFromUnsignedIntegral()
{
  static_assert(decltype(UnsignedBig{ static_cast<U8>(0U) })::gBits == 8U);
  static_assert(decltype(UnsignedBig{ static_cast<U16>(0U) })::gBits == 16U);
  static_assert(decltype(UnsignedBig{ static_cast<U32>(0U) })::gBits == 32U);
  static_assert(decltype(UnsignedBig{ static_cast<U64>(0U) })::gBits == 64U);
  static_assert(decltype(UnsignedBig{ static_cast<Element>(0U) })::gBits == gElementBits);
  static_assert(decltype(UnsignedBig{ static_cast<Wide>(0U) })::gBits == gWideBits);
}

/// deduction from signed integrals must drop the sign bit
void
testDeductionFromSignedIntegral()
{
  static_assert(decltype(UnsignedBig{ static_cast<I8>(0) })::gBits == 7U);
  static_assert(decltype(UnsignedBig{ static_cast<I16>(0) })::gBits == 15U);
  static_assert(decltype(UnsignedBig{ static_cast<I32>(0) })::gBits == 31U);
  static_assert(decltype(UnsignedBig{ static_cast<I64>(0) })::gBits == 63U);
}

/// deduction from element arrays must use the accumulated bit width
void
testDeductionFromElements()
{
  constexpr auto value = UnsignedBig{ std::array{
    static_cast<Element>(1),
    static_cast<Element>(2),
    static_cast<Element>(3),
  } };
  static_assert(decltype(value)::gBits == 3 * gElementBits);
}

/// small values must be stored as plain integers with minimal footprint
void
testIntDataStorageSelection()
{
  static_assert(decltype(UnsignedBig{ static_cast<U8>(0U) })::gIsIntData);
  static_assert(decltype(UnsignedBig{ static_cast<U64>(0U) })::gIsIntData);
  static_assert(decltype(UnsignedBig{ static_cast<Wide>(0U) })::gIsIntData);

  static_assert(sizeof(UnsignedBig{ static_cast<U8>(0U) }.mData) == 1);
  static_assert(sizeof(UnsignedBig{ static_cast<U16>(0U) }.mData) == 4);
  static_assert(sizeof(UnsignedBig{ static_cast<U32>(0U) }.mData) == 4);
  static_assert(sizeof(UnsignedBig{ static_cast<U64>(0U) }.mData) == 8);
  static_assert(sizeof(UnsignedBig{ static_cast<Element>(0U) }.mData) == gElementBytes);
  static_assert(sizeof(UnsignedBig{ static_cast<Wide>(0U) }.mData) == gWideBytes);
}

/// values beyond the widest integer must be stored as arrays
void
testArrayDataStorageSelection()
{
  constexpr auto value = UnsignedBig{ std::array{
    static_cast<Element>(1),
    static_cast<Element>(2),
    static_cast<Element>(3),
  } };
  static_assert(decltype(value)::gIsArrayData);
  static_assert(value.mData.size() == static_cast<Size>(3));
}

/// element arrays that still fit into an integer must collapse into int data
void
testSmallArrayCollapsesToIntData()
{
  constexpr auto value = UnsignedBig{ std::array{ 0x1U, 0x2U } };
  static_assert(decltype(value)::gIsIntData);
  static_assert(decltype(value)::gBits == 2 * gBitsOf<U32>);
  static_assert(value.mData == 0x2'0000'0001ULL);
}

} // namespace

int
main(int /* argc */, char** /* argv */)
{
  try {
    testDeductionFromUnsignedIntegral();
    testDeductionFromSignedIntegral();
    testDeductionFromElements();
    testIntDataStorageSelection();
    testArrayDataStorageSelection();
    testSmallArrayCollapsesToIntData();
  } catch (std::exception const&) {
    std::exit(1); // NOLINT(*-mt-unsafe)
  }
}

// NOLINTEND(*-magic-numbers)
