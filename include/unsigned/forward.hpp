#pragma once

// IWYU pragma: private, include <unsigned.hpp>

#include <common/int.hpp>
#include <common/type_traits.hpp>

#include <array>
#include <type_traits>

namespace big {

using Element = std::conditional_t<gU128Supported, U64, U32>;
using Wide    = std::conditional_t<gU128Supported, U128, U64>;
using Limb    = U32;

constexpr auto gElementBytes = gBytesOf<Element>;
constexpr auto gWideBytes    = gBytesOf<Wide>;
constexpr auto gLimbBytes    = gBytesOf<Limb>;

constexpr auto gElementBits = gBitsOf<Element>;
constexpr auto gWideBits    = gBitsOf<Wide>;
constexpr auto gLimbBits    = gBitsOf<Limb>;

// clang-format off
template<Size size> using Elements = std::array<Element, size>;
template<Size size> using Limbs    = std::array<Limb, size>;

template<typename T> concept IsElement  = Is<T, Element>;
template<typename T> concept IsWide     = Is<T, Wide>;
template<typename T> concept IsLimb     = Is<T, Limb>;
template<typename T> concept IsElements = IsArrayOf<T, Element>;
template<typename T> concept IsLimbs    = IsArrayOf<T, Limb>;
// clang-format on

constexpr auto gLimbIsElement = IsLimb<Element>;

template<Size bits>
class UnsignedBig;

template<typename T>
struct IsUnsignedBigHelper final : std::false_type
{};

template<Size bits>
struct IsUnsignedBigHelper<UnsignedBig<bits>> final : std::true_type
{
  constexpr static auto gBits        = bits;
  constexpr static auto gNumElements = (bits + gElementBits - 1) / gElementBits;

  // clang-format off
  struct ZeroData{};
  using Data = std::conditional_t<bits == 0, ZeroData,
               std::conditional_t<bits <= gU8Bits, U8,
               std::conditional_t<bits <= gU32Bits, U32,
               std::conditional_t<bits <= gElementBits, Element,
               std::conditional_t<bits <= gWideBits, Wide,
               Elements<gNumElements>>>>>>;
  // clang-format on

  constexpr static auto gIsZeroData  = Is<Data, ZeroData>;
  constexpr static auto gIsArrayData = Is<Data, Elements<gNumElements>>;
  constexpr static auto gIsIntData   = !gIsZeroData && !gIsArrayData;

  constexpr static auto gIsElementData = Is<Data, Element>;
  constexpr static auto gIsWideData    = Is<Data, Wide>;
  constexpr static auto gIsNarrowData  = gIsIntData && !gIsElementData && !gIsWideData;

  // consistency checks and assumptions
  static_assert(!gIsZeroData || gNumElements == 0);
  static_assert(!gIsIntData || gNumElements == 1 || gNumElements == 2);
  static_assert(!gIsArrayData || gNumElements >= 2);
  static_assert(!gIsNarrowData || gNumElements == 1);
  static_assert(!gIsElementData || gNumElements == 1);
  static_assert(!gIsWideData || gNumElements == 2);
};

/// exact match to use with auto to only accept exact match
template<typename T>
concept IsUnsignedBig = IsUnsignedBigHelper<std::remove_cvref_t<T>>::value;

/// a type that can be assigned safely from all the given unsigned types without truncation
template<IsUnsignedBig... Args>
using CommonUnsignedBig = UnsignedBig<std::max({ IsUnsignedBigHelper<std::remove_cvref_t<Args>>::gBits... })>;

/// describes values an UnsignedBig is explicitly constructible from
template<typename Value, Size bits>
concept IsUnsignedBigInit = []() consteval -> bool {
  using V = std::remove_cvref_t<Value>;

  if constexpr (IsUnsignedBig<V>) {
    return IsUnsignedBigHelper<V>::gBits <= bits;
  }

  else if constexpr (IsUnsignedIntegral<V> || IsElements<V> || IsLimbs<V>) {
    return gBitsOf<V> <= bits;
  }

  return false;
}();

/// describes values an UnsignedBig is explicitly constructible from only at compile time
/// these constructions are not generally safe, but can be checked at compile time to work for compile time values
template<typename Value, Size bits>
concept IsUnsignedBigConstevalInit = !IsUnsignedBigInit<Value, bits> && []() consteval -> bool {
  using V = std::remove_cvref_t<Value>;
  return IsUnsignedBig<V> || IsUnsignedIntegral<V> || IsSignedIntegral<V> || IsElements<V> || IsLimbs<V>;
}();

} // namespace big
