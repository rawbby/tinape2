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

  using UnsignedT   = UnsignedBig<bits>;
  using UnderlyingT = std::conditional_t<SignType::Runtime == signType, std::tuple<UnsignedT, bool>, std::tuple<UnsignedT>>;

  [[no_unique_address]] UnderlyingT mUnderlying{};

  constexpr SignedBig() = default;

  constexpr explicit SignedBig(
    UnsignedT const& value)
    requires(SignType::Runtime != signType)
    : mUnderlying(value)
  {
  }

  constexpr SignedBig(
    UnsignedT const& value,
    bool             sign)
    requires(signType == SignType::Runtime)
    : mUnderlying(value,
                  sign)
  {
  }

  constexpr SignedBig(
    SignTag<signType> /* tag */,
    UnsignedT const& value)
    requires(SignType::Runtime != signType)
    : mUnderlying(value)
  {
  }

  constexpr SignedBig(
    SignTag<signType> /* tag */,
    UnsignedT const& value,
    bool             sign)
    requires(SignType::Runtime == signType)
    : mUnderlying(value,
                  sign)
  {
  }

  template<SignType rhsSignType,
           Size     rhsBits>
    requires((rhsBits <= bits) && (SignType::Runtime == signType) && (rhsBits != bits || rhsSignType != signType))
  constexpr explicit SignedBig(
    SignedBig<rhsSignType,
              rhsBits> const& rhs)
    : mUnderlying(rhs.abs(),
                  rhs.sign())
  {
  }

  template<Size rhsBits>
    requires((rhsBits <= bits) && (SignType::Runtime != signType) && (rhsBits != bits))
  constexpr explicit SignedBig(
    SignedBig<signType,
              rhsBits> const& rhs)
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

template<SignType signType,
         Size     bits>
  requires(signType != SignType::Runtime)
SignedBig(SignTag<signType>,
          UnsignedBig<bits>) -> SignedBig<signType,
                                          bits>;

template<Size bits>
SignedBig(SignTag<SignType::Runtime>,
          UnsignedBig<bits>,
          bool) -> SignedBig<SignType::Runtime,
                             bits>;

template<Size bits>
SignedBig(UnsignedBig<bits>,
          bool) -> SignedBig<SignType::Runtime,
                             bits>;

} // namespace big
