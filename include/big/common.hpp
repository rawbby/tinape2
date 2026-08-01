#pragma once

#include <concepts>    // std::same_as
#include <cstddef>     // std::ptrdiff_t
#include <cstdint>     // std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t, std::int8_t, std::int16_t, std::int32_t, std::int64_t
#include <cstdlib>     // std::size_t
#include <type_traits> // std::remove_cvref_t

namespace big {

#if defined(__SIZEOF_INT128__) && !defined(BIG_NO_INT128)
constexpr auto gHasInt128 = true;
using Element             = std::uint64_t;
using Wide                = unsigned __int128; // NOLINT(*-runtime-int)
#else
constexpr auto gHasInt128 = false;
using Element             = std::uint32_t;
using Wide                = std::uint64_t;
#endif

using Size    = std::size_t;
using PtrDiff = std::ptrdiff_t;
using U8      = std::uint8_t;
using U16     = std::uint16_t;
using U32     = std::uint32_t;
using U64     = std::uint64_t;
using I8      = std::int8_t;
using I16     = std::int16_t;
using I32     = std::int32_t;
using I64     = std::int64_t;

template<typename T>
constexpr auto gBytesOf = static_cast<Size>(sizeof(T));

template<typename T>
constexpr auto gBitsOf = static_cast<Size>(gBytesOf<T> << 3U);

constexpr auto gElementBytes = gBytesOf<Element>;
constexpr auto gWideBytes    = gBytesOf<Wide>;
constexpr auto gSizeBytes    = gBytesOf<Size>;
constexpr auto gPtrDiffBytes = gBytesOf<PtrDiff>;
constexpr auto gU8Bytes      = gBytesOf<U8>;
constexpr auto gU16Bytes     = gBytesOf<U16>;
constexpr auto gU32Bytes     = gBytesOf<U32>;
constexpr auto gU64Bytes     = gBytesOf<U64>;
constexpr auto gI8Bytes      = gBytesOf<I8>;
constexpr auto gI16Bytes     = gBytesOf<I16>;
constexpr auto gI32Bytes     = gBytesOf<I32>;
constexpr auto gI64Bytes     = gBytesOf<I64>;

constexpr auto gElementBits = gBitsOf<Element>;
constexpr auto gWideBits    = gBitsOf<Wide>;
constexpr auto gSizeBits    = gBitsOf<Size>;
constexpr auto gPtrDiffBits = gBitsOf<PtrDiff>;
constexpr auto gU8Bits      = gBitsOf<U8>;
constexpr auto gU16Bits     = gBitsOf<U16>;
constexpr auto gU32Bits     = gBitsOf<U32>;
constexpr auto gU64Bits     = gBitsOf<U64>;
constexpr auto gI8Bits      = gBitsOf<I8>;
constexpr auto gI16Bits     = gBitsOf<I16>;
constexpr auto gI32Bits     = gBitsOf<I32>;
constexpr auto gI64Bits     = gBitsOf<I64>;

template<typename Lhs, typename Rhs>
concept Is = std::same_as<std::remove_cvref_t<Lhs>, std::remove_cvref_t<Rhs>>;

// clang-format off
template<typename T> concept IsElement = Is<T, Element>;
template<typename T> concept IsWide    = Is<T, Wide>;
template<typename T> concept IsSize    = Is<T, Size>;
template<typename T> concept IsPtrDiff = Is<T, PtrDiff>;
template<typename T> concept IsU8      = Is<T, U8>;
template<typename T> concept IsU16     = Is<T, U16>;
template<typename T> concept IsU32     = Is<T, U32>;
template<typename T> concept IsU64     = Is<T, U64>;
template<typename T> concept IsI8      = Is<T, I8>;
template<typename T> concept IsI16     = Is<T, I16>;
template<typename T> concept IsI32     = Is<T, I32>;
template<typename T> concept IsI64     = Is<T, I64>;
// clang-format on

[[nodiscard]] constexpr auto
loElement(
  IsWide auto value) -> Element
{
  return static_cast<Element>(value);
}

[[nodiscard]] constexpr auto
hiElement(
  IsWide auto value) -> Element
{
  return static_cast<Element>(value >> gElementBits);
}

enum class SignType : I8
{
  Positive = 1,
  Runtime  = 0,
  Negative = -1,
};

template<typename T>
  requires(std::unsigned_integral<T> || IsElement<T> || IsWide<T>)
constexpr auto gMask = static_cast<T>(-1);

template<typename T>
  requires(std::unsigned_integral<T> || IsElement<T> || IsWide<T>)
[[nodiscard]] constexpr auto
loMask(
  Size bits) -> T
{
  if (bits >= gBitsOf<T>) {
    return gMask<T>;
  }
  return static_cast<T>(static_cast<T>(T{ 1 } << bits) - T{ 1 });
}

template<typename T>
  requires(std::unsigned_integral<T> || IsElement<T> || IsWide<T>)
[[nodiscard]] constexpr auto
hiMask(
  Size bits) -> T
{
  if (bits >= gBitsOf<T>) {
    return gMask<T>;
  }
  return static_cast<T>(gMask<T> ^ static_cast<T>(gMask<T> >> bits));
}

template<typename T, Size bits>
constexpr auto gLoMask = loMask<T>(bits);

template<typename T, Size bits>
constexpr auto gHiMask = hiMask<T>(bits);

} // namespace big
