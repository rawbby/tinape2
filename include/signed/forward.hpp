#pragma once

// IWYU pragma: private, include <signed.hpp>

#include <common.hpp>
#include <unsigned.hpp>

#include <tuple>
#include <type_traits>

namespace big {

// NOLINTNEXTLINE(*-use-enum-class)
enum SignType : I8
{
  Negative = 0,
  Positive = 1,
  Runtime  = 2,
};

template<SignType signType, Size bits>
class SignedBig;

template<typename T>
struct IsSignedBigHelper final : std::false_type
{};

template<SignType signType, Size bits>
struct IsSignedBigHelper<SignedBig<signType, bits>> final : std::true_type
{
  using Unsigned   = UnsignedBig<bits>;
  using Underlying = std::conditional_t<signType == Runtime, std::tuple<Unsigned, bool>, std::tuple<Unsigned>>;
  using Data       = Unsigned::Data;

  constexpr static auto gBits          = Unsigned::gBits;
  constexpr static auto gNumElements   = Unsigned::gNumElements;
  constexpr static auto gIsZeroData    = Unsigned::gIsZeroData;
  constexpr static auto gIsArrayData   = Unsigned::gIsArrayData;
  constexpr static auto gIsIntData     = Unsigned::gIsIntData;
  constexpr static auto gIsElementData = Unsigned::gIsElementData;
  constexpr static auto gIsWideData    = Unsigned::gIsWideData;
  constexpr static auto gIsNarrowData  = Unsigned::gIsNarrowData;
  constexpr static auto gSignType      = signType;
};

template<typename T>
concept IsSignedBig = IsSignedBigHelper<std::remove_cvref_t<T>>::value;

// clang-format off
template <IsSignedBig... Ts>
using CommonSignedBig = SignedBig<
    ((Ts::gSignType == std::tuple_element_t<0, std::tuple<Ts...>>::gSignType) && ...)
        ? std::tuple_element_t<0, std::tuple<Ts...>>::gSignType
        : SignType::Runtime,
    CommonUnsignedBig<typename Ts::UnsignedBig...>::gBits>;
// clang-format on

/// describes values a SignedBig is explicitly constructible from without runtime errors
template<typename Value, SignType signType, Size bits>
concept IsSignedBigInit = []() consteval -> bool {
  using V = std::remove_cvref_t<Value>;

  if constexpr (IsSignedBig<V>) {
    return (signType == Runtime || signType == V::gSignType) && (V::gBits <= bits);
  }

  else if constexpr (IsUnsignedBig<V>) {
    return IsUnsignedBigHelper<V>::gBits <= bits;
  }

  else if constexpr (IsUnsignedIntegral<V> || IsElements<V> || IsLimbs<V>) {
    return gBitsOf<V> <= bits;
  }

  else if constexpr (IsSignedIntegral<V>) {
    return (signType == Runtime) && (gBitsOf<V> - 1 <= bits);
  }

  return false;
}();

/// describes values a SignedBig is explicitly constructible from only at compile time
/// these constructions are not generally safe, but can be checked at compile time to work for compile time values
template<typename Value, SignType signType, Size bits>
concept IsSignedBigConstevalInit = !IsSignedBigInit<Value, signType, bits> && []() consteval -> bool {
  using V = std::remove_cvref_t<Value>;
  return IsSignedBig<V> || IsUnsignedBig<V> || IsUnsignedIntegral<V> || IsSignedIntegral<V> || IsElements<V> || IsLimbs<V>;
}();

} // namespace big
