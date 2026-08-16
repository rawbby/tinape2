#include <common.hpp>
#include <signed.hpp>
#include <unsigned.hpp>

#include <array>
#include <cstdlib>
#include <exception>

// NOLINTBEGIN(*-magic-numbers)

namespace {

using namespace big;

/// deduction from unsigned integrals must use the full bit width and a positive sign type
void
testDeductionFromUnsignedIntegral()
{
  static_assert(decltype(SignedBig{ static_cast<U8>(0U) })::gBits == 8U);
  static_assert(decltype(SignedBig{ static_cast<U16>(0U) })::gBits == 16U);
  static_assert(decltype(SignedBig{ static_cast<U32>(0U) })::gBits == 32U);
  static_assert(decltype(SignedBig{ static_cast<U64>(0U) })::gBits == 64U);
  static_assert(decltype(SignedBig{ static_cast<Element>(0U) })::gBits == gElementBits);
  static_assert(decltype(SignedBig{ static_cast<Wide>(0U) })::gBits == gWideBits);

  static_assert(decltype(SignedBig{ static_cast<U8>(0U) })::gSignType == Positive);
}

/// deduction from signed integrals must drop the sign bit and use a runtime sign
void
testDeductionFromSignedIntegral()
{
  static_assert(decltype(SignedBig{ static_cast<I8>(0) })::gBits == 7U);
  static_assert(decltype(SignedBig{ static_cast<I16>(0) })::gBits == 15U);
  static_assert(decltype(SignedBig{ static_cast<I32>(0) })::gBits == 31U);
  static_assert(decltype(SignedBig{ static_cast<I64>(0) })::gBits == 63U);

  static_assert(decltype(SignedBig{ static_cast<I8>(0) })::gSignType == Runtime);
}

/// deduction with an explicit sign flag must use a runtime sign
void
testDeductionWithExplicitSign()
{
  static_assert(decltype(SignedBig{ static_cast<U8>(0U), true })::gSignType == Runtime);
  static_assert(decltype(SignedBig{ static_cast<U8>(0U), true })::gBits == 8U);
}

/// small values must be stored as plain integers with minimal footprint
void
testIntDataStorageSelection()
{
  static_assert(decltype(SignedBig{ static_cast<U8>(0U) })::gIsIntData);
  static_assert(decltype(SignedBig{ static_cast<Wide>(0U) })::gIsIntData);

  static_assert(sizeof(SignedBig{ static_cast<U8>(0U) }.abs().mData) == 1);
  static_assert(sizeof(SignedBig{ static_cast<U16>(0U) }.abs().mData) == 4);
  static_assert(sizeof(SignedBig{ static_cast<U32>(0U) }.abs().mData) == 4);
  static_assert(sizeof(SignedBig{ static_cast<U64>(0U) }.abs().mData) == 8);
  static_assert(sizeof(SignedBig{ static_cast<Element>(0U) }.abs().mData) == gElementBytes);
  static_assert(sizeof(SignedBig{ static_cast<Wide>(0U) }.abs().mData) == gWideBytes);
}

/// values beyond the widest integer must be stored as arrays
void
testArrayDataStorageSelection()
{
  constexpr auto value = SignedBig{ std::array{
    static_cast<Element>(1),
    static_cast<Element>(2),
    static_cast<Element>(3),
  } };
  static_assert(decltype(value)::gIsArrayData);
  static_assert(decltype(value)::gBits == 3 * gElementBits);
  static_assert(value.abs().mData.size() == static_cast<Size>(3));
}

/// element arrays that still fit into an integer must collapse into int data
void
testSmallArrayCollapsesToIntData()
{
  constexpr auto value = SignedBig{ std::array{ 0x1U, 0x2U } };
  static_assert(decltype(value)::gIsIntData);
  static_assert(decltype(value)::gBits == 2 * gBitsOf<U32>);
  static_assert(value.abs().mData == 0x2'0000'0001ULL);
}

} // namespace

int
main(int /* argc */, char** /* argv */)
{
  try {
    testDeductionFromUnsignedIntegral();
    testDeductionFromSignedIntegral();
    testDeductionWithExplicitSign();
    testIntDataStorageSelection();
    testArrayDataStorageSelection();
    testSmallArrayCollapsesToIntData();
  } catch (std::exception const&) {
    std::exit(1); // NOLINT(*-mt-unsafe)
  }
}

// NOLINTEND(*-magic-numbers)
