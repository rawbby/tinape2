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

template<Size bits, SignType signType>
struct SignedUnderlying
{
  // NOLINTNEXTLINE(*-identifier-naming)
  static constexpr bool mSign = signType == Positive;

  [[no_unique_address]] UnsignedBig<bits> mAbs{};
};

template<Size bits>
struct SignedUnderlying<bits, Runtime>
{
  [[no_unique_address]] UnsignedBig<bits> mAbs{};
  bool                                    mSign{};
};

template<SignType signType, Size bits>
class SignedBig;

namespace impl {

template<typename T>
struct IsSignedBigHelper final : std::false_type
{
  constexpr static auto gBits     = gBitsOf<T>;
  constexpr static auto gSignType = IsSignedIntegral<T> ? Runtime : Positive;
};

template<Size bits>
struct IsSignedBigHelper<UnsignedBig<bits>> final : std::false_type
{
  constexpr static auto gBits     = UnsignedBig<bits>::gBits;
  constexpr static auto gSignType = Positive;
};

template<SignType signType, Size bits>
struct IsSignedBigHelper<SignedBig<signType, bits>> final : std::true_type
{
  using Unsigned   = UnsignedBig<bits>;
  using Underlying = SignedUnderlying<bits, signType>;
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

/// concept for all runtime initializers to signed big
template<typename Value, SignType signType, Size bits>
concept IsSignedBigRhs =
  // comaptible signed big
  (impl::IsSignedBigHelper<std::remove_cvref_t<Value>>::value &&
   // with compatible sign
   (impl::IsSignedBigHelper<std::remove_cvref_t<Value>>::gSignType == signType || Runtime == signType) &&
   // with compatible bits
   impl::IsSignedBigHelper<std::remove_cvref_t<Value>>::gBits <= bits) ||

  // or compatible unsigned big or native type
  ((IsUnsignedBig<Value> || IsUnsignedIntegral<Value> || IsElements<Value> || IsLimbs<Value>) &&
   // with compatible bits
   impl::IsSignedBigHelper<std::remove_cvref_t<Value>>::gBits <= bits) ||

  // or compatible signed native type
  (IsSignedIntegral<Value> &&
   // with runtime sign
   signType == Runtime &&
   // with compatible bits
   impl::IsSignedBigHelper<std::remove_cvref_t<Value>>::gBits - 1 <= bits);

/// concept for all consteval but not runtime initializers to signed big
template<typename Value, SignType signType, Size bits>
concept IsSignedBigConstevalRhs =
  // non-runtime compatible type
  !IsSignedBigRhs<Value, signType, bits> &&
  // compatible signed big, unsigned big or native type
  (impl::IsSignedBigHelper<std::remove_cvref_t<Value>>::value || IsUnsignedBig<Value> || IsIntegral<Value> || IsElements<Value> || IsLimbs<Value>);

} // namespace impl

template<typename T>
concept IsSignedBig = impl::IsSignedBigHelper<std::remove_cvref_t<T>>::value;

// clang-format off
template <IsSignedBig... Ts>
using CommonSignedBig = SignedBig<
    ((Ts::gSignType == std::tuple_element_t<0, std::tuple<Ts...>>::gSignType) && ...)
        ? std::tuple_element_t<0, std::tuple<Ts...>>::gSignType
        : SignType::Runtime,
    CommonUnsignedBig<typename Ts::Unsigned...>::gBits>;
// clang-format on

} // namespace big
