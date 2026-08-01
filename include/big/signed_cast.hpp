#pragma once

#include <big/common.hpp>             // big::SignType
#include <big/signed_type_traits.hpp> // big::IsSignedBig
#include <big/unsigned_cast.hpp>      // big::castBigUnsafe
#include <optional>                   // std::optional

namespace big {

// truncation based cast without extensive
// runtime checking but simple (exact) masking
template<IsSignedBig ToSignedBig,
         SignType    fromSignType,
         Size        fromBits>
[[nodiscard]] constexpr auto
castBigUnsafe(
  SignedBig<fromSignType,
            fromBits> const& fromValue) -> ToSignedBig
{
  if constexpr (SignType::Runtime == ToSignedBig::gSignType) {
    return {
      castBigUnsafe<ToSignedBig::UnsignedT>(fromValue.abs()),
      fromValue.sign(),
    };

  } else {
    return {
      castBigUnsafe<ToSignedBig::UnsignedT>(fromValue.abs()),
      SignType::Positive == ToSignedBig::gSignType,
    };
  }
}

// safe casting returns a valid value if fromValue
// fits toValue or std::nullopt
template<IsSignedBig ToSignedBig,
         SignType    fromSignType,
         Size        fromBits>
[[nodiscard]] constexpr auto
castBigSafe(
  SignedBig<fromSignType,
            fromBits> const& fromValue) -> std::optional<ToSignedBig>
{
  using ToUnsigned          = ToSignedBig::UnsignedT;
  constexpr auto toSignType = ToSignedBig::gSignType;
  constexpr auto toSign     = SignType::Positive == toSignType;

  // if compile time simple case
  if constexpr (SignType::Runtime == toSignType) {
    if (auto const toAbs = castBigSafe<ToUnsigned>(fromValue.abs())) {
      return ToSignedBig{ *toAbs, fromValue.sign() };
    }
    return std::nullopt;
  }

  // if compile time sign inequality NOLINTNEXTLINE(*-branch-clone)
  else if constexpr (SignType::Runtime != fromSignType && toSignType != fromSignType) {
    if (fromValue.abs() == 0) {
      return ToSignedBig{ ToUnsigned{}, toSign };
    }
    return std::nullopt;
  }

  else {

    // if runtime sign inequality
    if (toSign != fromValue.sign()) {
      if (fromValue.abs() == 0) {
        return ToSignedBig{ ToUnsigned{}, toSign };
      }
      return std::nullopt;
    }

    // if runtime sign equality
    if (auto const toAbs = castBigSafe<ToUnsigned>(fromValue.abs())) {
      return ToSignedBig{ *toAbs, toSign };
    }
    return std::nullopt;
  }
}

} // namespace big
