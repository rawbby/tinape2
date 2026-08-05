#pragma once

#include <big/common.hpp>   // big::SignType, big::BigElement
#include <big/unsigned.hpp> // big::UnsignedBig

#include <tuple>       // std::tuple, std::get
#include <type_traits> // std::conditional_t

namespace big {

template<SignType>
struct SignTag
{};

template<SignType signType>
inline constexpr auto gSignTag = SignTag<signType>{};

inline constexpr auto gPositive = gSignTag<SignType::Positive>;
inline constexpr auto gRuntime  = gSignTag<SignType::Runtime>;
inline constexpr auto gNegative = gSignTag<SignType::Negative>;

template<SignType signType, Size bits>
struct SignedBig
{
  constexpr static auto gSignType = signType;
  constexpr static auto gBits     = bits;

  constexpr static auto gIsRuntimeSign   = gSignType == SignType::Runtime;
  constexpr static auto gIsntRuntimeSign = gSignType != SignType::Runtime;
  constexpr static auto gIsPositiveSign  = gSignType == SignType::Positive;
  constexpr static auto gIsNegativeSign  = gSignType == SignType::Negative;

  using UnsignedT   = UnsignedBig<bits>;
  using UnderlyingT = std::conditional_t<gIsRuntimeSign, std::tuple<UnsignedT, bool>, std::tuple<UnsignedT>>;

  [[no_unique_address]] UnderlyingT mUnderlying{};

  constexpr SignedBig() = default;

  constexpr explicit SignedBig(
    auto const& value)
    requires gIsntRuntimeSign && requires { UnsignedT{ value }; }
    : mUnderlying(value)
  {
  }

  constexpr SignedBig(
    auto const& value,
    bool        sign)
    requires gIsRuntimeSign && requires { UnsignedT{ value }; }
    : mUnderlying(value,
                  sign)
  {
  }

  constexpr SignedBig(
    SignTag<signType> /* tag */,
    auto const& value)
    requires gIsntRuntimeSign && requires { UnsignedT{ value }; }
    : mUnderlying(value)
  {
  }

  constexpr SignedBig(
    SignTag<signType> /* tag */,
    auto const& value,
    bool        sign)
    requires gIsRuntimeSign && requires { UnsignedT{ value }; }
    : mUnderlying(value,
                  sign)
  {
  }

  template<SignType rhsSignType,
           Size     rhsBits>
    requires(rhsBits != bits || rhsSignType != signType)
  constexpr explicit SignedBig(
    SignedBig<rhsSignType,
              rhsBits> const& rhs)
    requires gIsRuntimeSign && requires { UnsignedT{ rhs.abs() }; }
    : mUnderlying(rhs.abs(),
                  rhs.sign())
  {
  }

  template<Size rhsBits>
    requires(rhsBits != bits)
  constexpr explicit SignedBig(
    SignedBig<signType,
              rhsBits> const& rhs)
    requires gIsntRuntimeSign && requires { UnsignedT{ rhs.abs() }; }
    : mUnderlying(rhs.abs())
  {
  }

  [[nodiscard]] constexpr auto abs() const -> UnsignedT const&
  {
    return std::get<0>(mUnderlying);
  }

  [[nodiscard]] constexpr auto sign() const -> bool
  {
    if constexpr (signType != SignType::Runtime) {
      return signType == SignType::Positive;
    } else {
      return std::get<1>(mUnderlying);
    }
  }
};

template<SignType signType>
  requires(signType != SignType::Runtime)
SignedBig(SignTag<signType>,
          auto const& value) -> SignedBig<signType,
                                          decltype(UnsignedBig{ value })::gBits>;

SignedBig(SignTag<SignType::Runtime>,
          auto const& value,
          bool) -> SignedBig<SignType::Runtime,
                             decltype(UnsignedBig{ value })::gBits>;

SignedBig(auto const& value,
          bool) -> SignedBig<SignType::Runtime,
                             decltype(UnsignedBig{ value })::gBits>;

} // namespace big
