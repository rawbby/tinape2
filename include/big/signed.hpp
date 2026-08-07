#pragma once

#include <big/common.hpp>   // big::SignType, big::BigElement
#include <big/unsigned.hpp> // big::UnsignedBig
#include <common/tag.hpp>   // Tag, gTag

#include <tuple>       // std::tuple, std::get
#include <type_traits> // std::conditional_t

namespace big {

template<SignType signType, Size bits>
struct GenericSigned
{
  [[no_unique_address]] std::conditional_t<signType == Runtime, std::tuple<UnsignedBig<bits>, bool>, std::tuple<UnsignedBig<bits>>> mUnderlying{};

  constexpr GenericSigned()                     = default;
  constexpr ~GenericSigned()                    = default;
  constexpr GenericSigned(GenericSigned&&)      = default;
  constexpr GenericSigned(GenericSigned const&) = default;

  constexpr auto operator=(GenericSigned&&) -> GenericSigned&      = default;
  constexpr auto operator=(GenericSigned const&) -> GenericSigned& = default;

  [[nodiscard]] constexpr auto abs() const -> UnsignedBig<bits> const&
  {
    return std::get<0>(mUnderlying);
  }

  [[nodiscard]] constexpr auto sign() const -> bool
  {
    if constexpr (signType == Runtime) {
      return std::get<1>(mUnderlying);
    }
    return signType == Positive;
  }
};

} // namespace big
