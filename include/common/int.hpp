#pragma once

// IWYU pragma: private, include <common.hpp>

#include <common/type_traits.hpp> // Is, IsAny

#include <concepts> // unsigned_integral, std::signed_integral
#include <cstddef>  // std::ptrdiff_t
#include <cstdint>  // std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t, std::int8_t, std::int16_t, std::int32_t, std::int64_t
#include <cstdlib>  // std::size_t
#include <stdexcept>
#include <type_traits> // std::remove_cvref_t

struct UnavailableIntegral;

using Size    = std::size_t;
using PtrDiff = std::ptrdiff_t;

using U8  = std::uint8_t;
using U16 = std::uint16_t;
using U32 = std::uint32_t;
using U64 = std::uint64_t;

using I8  = std::int8_t;
using I16 = std::int16_t;
using I32 = std::int32_t;
using I64 = std::int64_t;

#if defined(__SIZEOF_INT128__) && !defined(BIG_NO_INT128)
constexpr auto gU128Supported = true;
constexpr auto gI128Supported = true;

// NOLINTBEGIN(*-runtime-int)
using U128 = unsigned __int128;
using I128 = __int128;
// NOLINTEND(*-runtime-int)
#else
constexpr auto gU128Supported = false;
constexpr auto gI128Supported = false;

using U128 = UnavailableIntegral;
using I128 = UnavailableIntegral;
#endif

// NOLINTBEGIN(*-magic-numbers)
// clang-format off

template<typename T> concept IsSize    = Is<T, Size>;
template<typename T> concept IsPtrDiff = Is<T, PtrDiff>;

template<typename T> concept IsU8   = (std::unsigned_integral<std::remove_cvref_t<T>>&&sizeof(T)==1)  || Is<T,U8>;
template<typename T> concept IsU16  = (std::unsigned_integral<std::remove_cvref_t<T>>&&sizeof(T)==2)  || Is<T,U16>;
template<typename T> concept IsU32  = (std::unsigned_integral<std::remove_cvref_t<T>>&&sizeof(T)==4)  || Is<T,U32>;
template<typename T> concept IsU64  = (std::unsigned_integral<std::remove_cvref_t<T>>&&sizeof(T)==8)  || Is<T,U64>;
template<typename T> concept IsU128 = (std::unsigned_integral<std::remove_cvref_t<T>>&&sizeof(T)==16) || (gU128Supported&&Is<T,U128>);

template<typename T> concept IsI8   = (std::signed_integral<std::remove_cvref_t<T>>&&sizeof(T)==1)  || Is<T,I8>;
template<typename T> concept IsI16  = (std::signed_integral<std::remove_cvref_t<T>>&&sizeof(T)==2)  || Is<T,I16>;
template<typename T> concept IsI32  = (std::signed_integral<std::remove_cvref_t<T>>&&sizeof(T)==4)  || Is<T,I32>;
template<typename T> concept IsI64  = (std::signed_integral<std::remove_cvref_t<T>>&&sizeof(T)==8)  || Is<T,I64>;
template<typename T> concept IsI128 = (std::signed_integral<std::remove_cvref_t<T>>&&sizeof(T)==16) || (gI128Supported&&Is<T,I128>);

template<typename T> concept IsUnsignedIntegral = std::unsigned_integral<std::remove_cvref_t<T>> || IsAny<T,U8,U16,U32,U64> || IsU128<T>;
template<typename T> concept IsSignedIntegral   = std::signed_integral<std::remove_cvref_t<T>>   || IsAny<T,I8,I16,I32,I64> || IsI128<T>;
template<typename T> concept IsIntegral         = IsUnsignedIntegral<T> || IsSignedIntegral<T>;

template<typename T> constexpr auto gBytesOf = static_cast<Size>(sizeof(std::remove_cvref_t<T>));
template<typename T> constexpr auto gBitsOf  = static_cast<Size>(gBytesOf<T> << 3U);

template<typename Value> constexpr auto bytesOf(Value /* value */) -> Size { return gBytesOf<Value>; }
template<typename Value> constexpr auto bitsOf(Value /* value */) -> Size { return gBitsOf<Value>; }

// clang-format on
// NOLINTEND(*-magic-numbers)

constexpr auto gSizeBytes    = gBytesOf<Size>;
constexpr auto gPtrDiffBytes = gBytesOf<PtrDiff>;

constexpr auto gU8Bytes  = gBytesOf<U8>;
constexpr auto gU16Bytes = gBytesOf<U16>;
constexpr auto gU32Bytes = gBytesOf<U32>;
constexpr auto gU64Bytes = gBytesOf<U64>;

constexpr auto gI8Bytes  = gBytesOf<I8>;
constexpr auto gI16Bytes = gBytesOf<I16>;
constexpr auto gI32Bytes = gBytesOf<I32>;
constexpr auto gI64Bytes = gBytesOf<I64>;

constexpr auto gSizeBits    = gBitsOf<Size>;
constexpr auto gPtrDiffBits = gBitsOf<PtrDiff>;

constexpr auto gU8Bits  = gBitsOf<U8>;
constexpr auto gU16Bits = gBitsOf<U16>;
constexpr auto gU32Bits = gBitsOf<U32>;
constexpr auto gU64Bits = gBitsOf<U64>;

constexpr auto gI8Bits  = gBitsOf<I8>;
constexpr auto gI16Bits = gBitsOf<I16>;
constexpr auto gI32Bits = gBitsOf<I32>;
constexpr auto gI64Bits = gBitsOf<I64>;

#if defined(__SIZEOF_INT128__) && !defined(BIG_NO_INT128)
constexpr auto gU128Bytes = gBytesOf<U128>;
constexpr auto gU128Bits  = gBitsOf<U128>;
constexpr auto gI128Bytes = gBytesOf<I128>;
constexpr auto gI128Bits  = gBitsOf<I128>;
#else
constexpr auto gU128Bytes = static_cast<Size>(0);
constexpr auto gU128Bits  = static_cast<Size>(0);
constexpr auto gI128Bytes = static_cast<Size>(0);
constexpr auto gI128Bits  = static_cast<Size>(0);
#endif

// clang-format off
template<Size bits>
using U =
  std::conditional_t<bits == gU8Bits,   U8,
  std::conditional_t<bits == gU16Bits,  U16,
  std::conditional_t<bits == gU32Bits,  U32,
  std::conditional_t<bits == gU64Bits,  U64,
  std::conditional_t<bits == gU128Bits, U128,
  UnavailableIntegral>>>>>;
template<Size bits>
using I =
  std::conditional_t<bits == gI8Bits,   I8,
  std::conditional_t<bits == gI16Bits,  I16,
  std::conditional_t<bits == gI32Bits,  I32,
  std::conditional_t<bits == gI64Bits,  I64,
  std::conditional_t<bits == gI128Bits, I128,
  UnavailableIntegral>>>>>;
// clang-format on

template<IsIntegral Value>
using MakeUnsigned = U<gBitsOf<Value>>;

template<IsIntegral Value>
constexpr auto
makeUnsigned(Value value) -> MakeUnsigned<Value>
{
  if (std::is_constant_evaluated()) {
    if (value < 0) {
      throw std::logic_error{ "undefined behaviour: can not cast negative value unsigned value" };
    }
  }
  return static_cast<MakeUnsigned<Value>>(value);
}

namespace static_test {

static_assert(sizeof(U8) == sizeof(I8));
static_assert(sizeof(U16) == sizeof(I16));
static_assert(sizeof(U32) == sizeof(I32));
static_assert(sizeof(U64) == sizeof(I64));

// static test integral concept

static_assert(IsIntegral<Size>);
static_assert(IsIntegral<PtrDiff>);

static_assert(IsIntegral<U8>);
static_assert(IsIntegral<U16>);
static_assert(IsIntegral<U32>);
static_assert(IsIntegral<U64>);
static_assert(!gU128Supported || IsIntegral<U128>);

static_assert(IsIntegral<I8>);
static_assert(IsIntegral<I16>);
static_assert(IsIntegral<I32>);
static_assert(IsIntegral<I64>);
static_assert(!gI128Supported || IsIntegral<I128>);

// static test signed integral concept

static_assert(!IsSignedIntegral<Size>);
static_assert(IsSignedIntegral<PtrDiff>);

static_assert(!IsSignedIntegral<U8>);
static_assert(!IsSignedIntegral<U16>);
static_assert(!IsSignedIntegral<U32>);
static_assert(!IsSignedIntegral<U64>);
static_assert(!gU128Supported || !IsSignedIntegral<U128>);

static_assert(IsSignedIntegral<I8>);
static_assert(IsSignedIntegral<I16>);
static_assert(IsSignedIntegral<I32>);
static_assert(IsSignedIntegral<I64>);
static_assert(!gI128Supported || IsSignedIntegral<I128>);

// static test unsigned integral concept

static_assert(IsUnsignedIntegral<Size>);
static_assert(!IsUnsignedIntegral<PtrDiff>);

static_assert(IsUnsignedIntegral<U8>);
static_assert(IsUnsignedIntegral<U16>);
static_assert(IsUnsignedIntegral<U32>);
static_assert(IsUnsignedIntegral<U64>);
static_assert(!gU128Supported || IsUnsignedIntegral<U128>);

static_assert(!IsUnsignedIntegral<I8>);
static_assert(!IsUnsignedIntegral<I16>);
static_assert(!IsUnsignedIntegral<I32>);
static_assert(!IsUnsignedIntegral<I64>);
static_assert(!gI128Supported || !IsUnsignedIntegral<I128>);

// static test I and U utilities

// clang-format off
static_assert(Is<U<gU8Bits>, U8>);
static_assert(Is<U<gU16Bits>, U16>);
static_assert(Is<U<gU32Bits>, U32>);
static_assert(Is<U<gU64Bits>, U64>);
static_assert(Is<U<gU128Bits>, U128>);
static_assert(Is<I<gI8Bits>, I8>);
static_assert(Is<I<gI16Bits>, I16>);
static_assert(Is<I<gI32Bits>, I32>);
static_assert(Is<I<gI64Bits>, I64>);
static_assert(Is<I<gI128Bits>, I128>);
// clang-format on

} // namespace static_test
