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

} // namespace impl

template<typename T>
concept IsSignedBig = impl::IsSignedBigHelper<std::remove_cvref_t<T>>::value;

// clang-format off
template <IsSignedBig... Ts>
using CommonSignedBig = SignedBig<
    ((Ts::gSignType == std::tuple_element_t<0, std::tuple<Ts...>>::gSignType) && ...)
        ? std::tuple_element_t<0, std::tuple<Ts...>>::gSignType
        : SignType::Runtime,
    CommonUnsignedBig<typename Ts::UnsignedBig...>::gBits>;
// clang-format on

} // namespace big
