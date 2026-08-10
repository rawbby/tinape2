#include <signed.hpp>

#include <common/int.hpp>

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

    constexpr auto bigUint8   = SignedBig{ uint8 };
    constexpr auto bigUint16  = SignedBig{ uint16 };
    constexpr auto bigUint32  = SignedBig{ uint32 };
    constexpr auto bigUint64  = SignedBig{ uint64 };
    constexpr auto bigElement = SignedBig{ element };
    constexpr auto bigWide    = SignedBig{ wide };
    constexpr auto bigInt8    = SignedBig{ int8 };
    constexpr auto bigInt16   = SignedBig{ int16 };
    constexpr auto bigInt32   = SignedBig{ int32 };
    constexpr auto bigInt64   = SignedBig{ int64 };

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

    static_assert(sizeof(bigUint8.abs().mData) == 1);
    static_assert(sizeof(bigUint16.abs().mData) == 4);
    static_assert(sizeof(bigUint32.abs().mData) == 4);
    static_assert(sizeof(bigUint64.abs().mData) == 8);
    static_assert(sizeof(bigElement.abs().mData) == gElementBytes);
    static_assert(sizeof(bigWide.abs().mData) == gWideBytes);
    static_assert(sizeof(bigInt8.abs().mData) == 1);
    static_assert(sizeof(bigInt16.abs().mData) == 4);
    static_assert(sizeof(bigInt32.abs().mData) == 4);
    static_assert(sizeof(bigInt64.abs().mData) == 8);

    static_assert(bigUint8.abs().mData == 8U);
    static_assert(bigUint16.abs().mData == 16U);
    static_assert(bigUint32.abs().mData == 32U);
    static_assert(bigUint64.abs().mData == 64U);
    static_assert(bigElement.abs().mData == gElementBits);
    static_assert(bigWide.abs().mData == gWideBits);
    static_assert(bigInt8.abs().mData == 7U);
    static_assert(bigInt16.abs().mData == 15U);
    static_assert(bigInt32.abs().mData == 31U);
    static_assert(bigInt64.abs().mData == 63U);

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

    constexpr auto overflow8 = SignedBig{ U8{ 255U } } + SignedBig<1>{ U8{ 1U } };
    static_assert(decltype(overflow8)::gIsIntData);
    static_assert(decltype(overflow8)::gBits == 9);
    static_assert(sizeof(overflow8.mData) == 4);
    static_assert(overflow8.mData == 256U);
    static_assert(overflow8 < 257U && overflow8 == 256U && overflow8 > 255U);

    constexpr auto arrayBig3 = SignedBig{ std::array{
      static_cast<Element>(1),
      static_cast<Element>(2),
      static_cast<Element>(3),
    } };
    static_assert(decltype(arrayBig3)::gIsArrayData);
    static_assert(decltype(arrayBig3)::gBits == 3 * gElementBits);
    static_assert(arrayBig3.abs().mData.size() == static_cast<Size>(3));
    static_assert(arrayBig3.abs().mData[0] == static_cast<Element>(1));
    static_assert(arrayBig3.abs().mData[1] == static_cast<Element>(2));
    static_assert(arrayBig3.abs().mData[2] == static_cast<Element>(3));
    static_assert(arrayBig3.abs().mData[0] == arrayBig3.abs().element(0));
    static_assert(arrayBig3.abs().mData[1] == arrayBig3.abs().element(1));
    static_assert(arrayBig3.abs().mData[2] == arrayBig3.abs().element(2));
    static_assert(arrayBig3 > 3);
    static_assert(arrayBig3 + 1 > arrayBig3);
    static_assert(arrayBig3 - 1 < arrayBig3);
    static_assert(arrayBig3 + 1 - 1 == arrayBig3);

    constexpr auto u32Big2 = SignedBig{ std::array{ 0x1U, 0x2U } };
    static_assert(decltype(u32Big2)::gIsIntData);
    static_assert(decltype(u32Big2)::gBits == 2 * gBitsOf<U32>);
    static_assert(u32Big2.abs().mData == 0x2'0000'0001ULL);

    constexpr auto u32Big9 = SignedBig{ std::array{
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

    constexpr auto u32Big9inc = SignedBig{ std::array{ 0x0U, 0x0U, 0x0U, 0x0U, 0x0U, 0x0U, 0x0U, 0x0U, 0x0U, 0x1U } };
    static_assert(decltype(u32Big9)::gIsArrayData);
    static_assert(decltype(u32Big9)::gBits == 9 * gBitsOf<U32>);

    constexpr auto overflow288 = u32Big9 + SignedBig<1>{ 1 };
    static_assert(decltype(overflow288)::gIsArrayData);
    static_assert(decltype(overflow288)::gBits == (9 * gBitsOf<U32>)+1);
    static_assert(overflow288.element(0) == 0U);
    static_assert(overflow288.element(1) == 0U);

    static_assert(decltype(u32Big9inc)::gIsArrayData);
    static_assert(decltype(u32Big9inc)::gBits == 10 * gBitsOf<U32>);
    static_assert(overflow288 == u32Big9inc);
    static_assert(overflow288 - 1 == u32Big9);

    static_assert(decltype(u32Big9inc)::gBits == (10 * gBitsOf<U32>));

    // static_assert(decltype(gUnsignedBig<u32Big9inc>)::gBits == (9 * gBitsOf<U32>)+1);
    // static_assert(decltype(gUnsignedBig<u32Big9inc - 1>)::gBits == 9 * gBitsOf<U32>);
    // static_assert(decltype(gUnsignedBig<u32Big9>)::gBits == 9 * gBitsOf<U32>);

    // static_assert(decltype(gUnsignedBig<0>)::gBits == 0);
    // static_assert(decltype(gUnsignedBig<1>)::gBits == 1);
    // static_assert(decltype(gUnsignedBig<2>)::gBits == 2);
    // static_assert(decltype(gUnsignedBig<3>)::gBits == 2);
    // static_assert(decltype(gUnsignedBig<4>)::gBits == 3);
    // static_assert(decltype(gUnsignedBig<5>)::gBits == 3);
    // static_assert(decltype(gUnsignedBig<6>)::gBits == 3);
    // static_assert(decltype(gUnsignedBig<7>)::gBits == 3);
    // static_assert(decltype(gUnsignedBig<8>)::gBits == 4);

    // static_assert(decltype(gUnsignedBig<0x7FFF'FFFFU>)::gBits == 31);
    // static_assert(decltype(gUnsignedBig<0x8000'0000U>)::gBits == 32);
    // static_assert(decltype(gUnsignedBig<0x8000'0001U>)::gBits == 32);
    // static_assert(decltype(gUnsignedBig<0xFFFF'FFFFULL>)::gBits == 32);
    // static_assert(decltype(gUnsignedBig<0x1'0000'0000ULL>)::gBits == 33);
    // static_assert(decltype(gUnsignedBig<0x1'0000'0001ULL>)::gBits == 33);
    // static_assert(decltype(gUnsignedBig<0x7FFF'FFFF'FFFF'FFFFULL>)::gBits == 63);
    // static_assert(decltype(gUnsignedBig<0x8000'0000'0000'0000ULL>)::gBits == 64);
    // static_assert(decltype(gUnsignedBig<0x8000'0000'0000'0001ULL>)::gBits == 64);
    // static_assert(decltype(gUnsignedBig<0xFFFF'FFFF'FFFF'FFFFULL>)::gBits == 64);

  } catch (std::exception const&) {
    std::exit(1); // NOLINT(*-mt-unsafe)
  }
}

// NOLINTEND(*-magic-numbers)
