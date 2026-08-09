#include <big/common.hpp>
#include <big/unsigned.hpp>
#include <big/unsigned_arithmetic.hpp>
#include <big/unsigned_compare.hpp>
#include <big/unsigned_constant.hpp>

#include <common/int.hpp>

#include <array>
#include <cstdlib>
#include <exception>

// NOLINTBEGIN(*-magic-numbers)

int
main(int /* argc */, char** /* argv */)
{
  using namespace big;

  try {
    constexpr auto uint8   = static_cast<U8>(8U);
    constexpr auto uint16  = static_cast<U16>(16U);
    constexpr auto uint32  = static_cast<U32>(32U);
    constexpr auto uint64  = static_cast<U64>(64U);
    constexpr auto element = static_cast<Element>(64U);
    constexpr auto wide    = static_cast<Wide>(128U);
    constexpr auto int8    = static_cast<I8>(7);
    constexpr auto int16   = static_cast<I16>(15);
    constexpr auto int32   = static_cast<I32>(31);
    constexpr auto int64   = static_cast<I64>(63);

    constexpr auto bigUint8   = UnsignedBig{ uint8 };
    constexpr auto bigUint16  = UnsignedBig{ uint16 };
    constexpr auto bigUint32  = UnsignedBig{ uint32 };
    constexpr auto bigUint64  = UnsignedBig{ uint64 };
    constexpr auto bigElement = UnsignedBig{ element };
    constexpr auto bigWide    = UnsignedBig{ wide };
    constexpr auto bigInt8    = UnsignedBig{ int8 };
    constexpr auto bigInt16   = UnsignedBig{ int16 };
    constexpr auto bigInt32   = UnsignedBig{ int32 };
    constexpr auto bigInt64   = UnsignedBig{ int64 };

    static_assert(decltype(bigUint8)::gIsIntData);
    static_assert(decltype(bigUint16)::gIsIntData);
    static_assert(decltype(bigUint32)::gIsIntData);
    static_assert(decltype(bigUint64)::gIsIntData);
    static_assert(decltype(bigElement)::gIsIntData);
    static_assert(decltype(bigWide)::gIsIntData);
    static_assert(decltype(bigInt8)::gIsIntData);
    static_assert(decltype(bigInt16)::gIsIntData);
    static_assert(decltype(bigInt32)::gIsIntData);
    static_assert(decltype(bigInt64)::gIsIntData);

    static_assert(decltype(bigUint8)::gBits == 8U);
    static_assert(decltype(bigUint16)::gBits == 16U);
    static_assert(decltype(bigUint32)::gBits == 32U);
    static_assert(decltype(bigUint64)::gBits == 64U);
    static_assert(decltype(bigElement)::gBits == gElementBits);
    static_assert(decltype(bigWide)::gBits == gWideBits);
    static_assert(decltype(bigInt8)::gBits == 7U);
    static_assert(decltype(bigInt16)::gBits == 15U);
    static_assert(decltype(bigInt32)::gBits == 31U);
    static_assert(decltype(bigInt64)::gBits == 63U);

    static_assert(sizeof(bigUint8.mData) == 1);
    static_assert(sizeof(bigUint16.mData) == 4);
    static_assert(sizeof(bigUint32.mData) == 4);
    static_assert(sizeof(bigUint64.mData) == 8);
    static_assert(sizeof(bigElement.mData) == gElementBytes);
    static_assert(sizeof(bigWide.mData) == gWideBytes);
    static_assert(sizeof(bigInt8.mData) == 1);
    static_assert(sizeof(bigInt16.mData) == 4);
    static_assert(sizeof(bigInt32.mData) == 4);
    static_assert(sizeof(bigInt64.mData) == 8);

    static_assert(bigUint8.mData == 8U);
    static_assert(bigUint16.mData == 16U);
    static_assert(bigUint32.mData == 32U);
    static_assert(bigUint64.mData == 64U);
    static_assert(bigElement.mData == gElementBits);
    static_assert(bigWide.mData == gWideBits);
    static_assert(bigInt8.mData == 7U);
    static_assert(bigInt16.mData == 15U);
    static_assert(bigInt32.mData == 31U);
    static_assert(bigInt64.mData == 63U);

    static_assert(bigUint8 == 8U);
    static_assert(bigUint16 == 16U);
    static_assert(bigUint32 == 32U);
    static_assert(bigUint64 == 64U);
    static_assert(bigElement == gElementBits);
    static_assert(bigWide == gWideBits);
    static_assert(bigInt8 == 7U);
    static_assert(bigInt16 == 15U);
    static_assert(bigInt32 == 31U);
    static_assert(bigInt64 == 63U);

    constexpr auto overflow8 = UnsignedBig{ U8{ 255U } } + UnsignedBig<1>{ U8{ 1U } };
    static_assert(decltype(overflow8)::gIsIntData);
    static_assert(decltype(overflow8)::gBits == 9);
    static_assert(sizeof(overflow8.mData) == 4);
    static_assert(overflow8.mData == 256U);
    static_assert(overflow8 < 257U && overflow8 == 256U && overflow8 > 255U);

    constexpr auto arrayBig3 = UnsignedBig{ std::array{
      static_cast<Element>(1),
      static_cast<Element>(2),
      static_cast<Element>(3),
    } };
    static_assert(decltype(arrayBig3)::gIsArrayData);
    static_assert(decltype(arrayBig3)::gBits == 3 * gElementBits);
    static_assert(arrayBig3.mData.size() == static_cast<Size>(3));
    static_assert(arrayBig3.mData[0] == static_cast<Element>(1));
    static_assert(arrayBig3.mData[1] == static_cast<Element>(2));
    static_assert(arrayBig3.mData[2] == static_cast<Element>(3));
    static_assert(arrayBig3.mData[0] == arrayBig3.element(0));
    static_assert(arrayBig3.mData[1] == arrayBig3.element(1));
    static_assert(arrayBig3.mData[2] == arrayBig3.element(2));
    static_assert(arrayBig3 > 3);
    static_assert(arrayBig3 + 1 > arrayBig3);
    static_assert(arrayBig3 - 1 < arrayBig3);
    static_assert(arrayBig3 + 1 - 1 == arrayBig3);

    constexpr auto u32Big2 = UnsignedBig{ std::array{ 0x1U, 0x2U } };
    static_assert(decltype(u32Big2)::gIsIntData);
    static_assert(decltype(u32Big2)::gBits == 2 * gBitsOf<U32>);
    static_assert(u32Big2.mData == 0x2'0000'0001ULL);

    constexpr auto u32Big9 = UnsignedBig{ std::array{
      0xFFFFFFFFU,
      0xFFFFFFFFU,
      0xFFFFFFFFU,
      0xFFFFFFFFU,
      0xFFFFFFFFU,
      0xFFFFFFFFU,
      0xFFFFFFFFU,
      0xFFFFFFFFU,
      0xFFFFFFFFU,
    } };
    static_assert(decltype(u32Big9)::gIsArrayData);
    static_assert(decltype(u32Big9)::gBits == 9 * gBitsOf<U32>);

    constexpr auto u32Big9inc = UnsignedBig{ std::array{ 0x0U, 0x0U, 0x0U, 0x0U, 0x0U, 0x0U, 0x0U, 0x0U, 0x0U, 0x1U } };
    static_assert(decltype(u32Big9)::gIsArrayData);
    static_assert(decltype(u32Big9)::gBits == 9 * gBitsOf<U32>);

    constexpr auto overflow288 = u32Big9 + UnsignedBig<1>{ 1 };
    static_assert(decltype(overflow288)::gIsArrayData);
    static_assert(decltype(overflow288)::gBits == (9 * gBitsOf<U32>)+1);
    static_assert(overflow288.element(0) == 0U);
    static_assert(overflow288.element(1) == 0U);

    static_assert(decltype(u32Big9inc)::gIsArrayData);
    static_assert(decltype(u32Big9inc)::gBits == 10 * gBitsOf<U32>);
    static_assert(overflow288 == u32Big9inc);
    static_assert(overflow288 - 1 == u32Big9);

    static_assert(decltype(u32Big9inc)::gBits == (10 * gBitsOf<U32>));
    static_assert(decltype(gUnsignedBig<u32Big9inc>)::gBits == (9 * gBitsOf<U32>)+1);
    static_assert(decltype(gUnsignedBig<u32Big9inc - 1>)::gBits == 9 * gBitsOf<U32>);
    static_assert(decltype(gUnsignedBig<u32Big9>)::gBits == 9 * gBitsOf<U32>);

    static_assert(decltype(gUnsignedBig<0>)::gBits == 0);
    static_assert(decltype(gUnsignedBig<1>)::gBits == 1);
    static_assert(decltype(gUnsignedBig<2>)::gBits == 2);
    static_assert(decltype(gUnsignedBig<3>)::gBits == 2);
    static_assert(decltype(gUnsignedBig<4>)::gBits == 3);
    static_assert(decltype(gUnsignedBig<5>)::gBits == 3);
    static_assert(decltype(gUnsignedBig<6>)::gBits == 3);
    static_assert(decltype(gUnsignedBig<7>)::gBits == 3);
    static_assert(decltype(gUnsignedBig<8>)::gBits == 4);

    static_assert(decltype(gUnsignedBig<0x7FFF'FFFFU>)::gBits == 31);
    static_assert(decltype(gUnsignedBig<0x8000'0000U>)::gBits == 32);
    static_assert(decltype(gUnsignedBig<0x8000'0001U>)::gBits == 32);
    static_assert(decltype(gUnsignedBig<0xFFFF'FFFFULL>)::gBits == 32);
    static_assert(decltype(gUnsignedBig<0x1'0000'0000ULL>)::gBits == 33);
    static_assert(decltype(gUnsignedBig<0x1'0000'0001ULL>)::gBits == 33);
    static_assert(decltype(gUnsignedBig<0x7FFF'FFFF'FFFF'FFFFULL>)::gBits == 63);
    static_assert(decltype(gUnsignedBig<0x8000'0000'0000'0000ULL>)::gBits == 64);
    static_assert(decltype(gUnsignedBig<0x8000'0000'0000'0001ULL>)::gBits == 64);
    static_assert(decltype(gUnsignedBig<0xFFFF'FFFF'FFFF'FFFFULL>)::gBits == 64);

  } catch (std::exception const&) {
    std::exit(1); // NOLINT(*-mt-unsafe)
  }
}

// NOLINTEND(*-magic-numbers)
