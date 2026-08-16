#pragma once

// IWYU pragma: private, include <signed.hpp>

#include <signed/declaration.hpp>
#include <signed/forward.hpp>

#include <common.hpp>
#include <unsigned.hpp>

#include <optional>

namespace big {

// truncation based cast without extensive
// runtime checking but simple (exact) masking
template<IsSignedBig ToSignedBig, SignType fromSignType, Size fromBits>
[[nodiscard]] constexpr auto
castBigUnsafe(SignedBig<fromSignType, fromBits> const& fromValue) -> ToSignedBig
{
  return ToSignedBig{
    castBigUnsafe<typename ToSignedBig::Unsigned>(fromValue.abs()),
    fromValue.sign(),
  };
}

// safe casting returns a valid value if fromValue
// fits toValue or std::nullopt
template<IsSignedBig ToSignedBig, SignType fromSignType, Size fromBits>
[[nodiscard]] constexpr auto
castBigSafe(SignedBig<fromSignType, fromBits> const& fromValue) -> std::optional<ToSignedBig>
{
  using ToUnsigned          = ToSignedBig::Unsigned;
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
      return ToSignedBig{ gTag<toSignType>, ToUnsigned{} };
    }
    return std::nullopt;
  }

  else {

    // if runtime sign inequality
    if (toSign != fromValue.sign()) {
      if (fromValue.abs() == 0) {
        return ToSignedBig{ gTag<toSignType>, ToUnsigned{} };
      }
      return std::nullopt;
    }

    // if runtime sign equality
    if (auto const toAbs = castBigSafe<ToUnsigned>(fromValue.abs())) {
      return ToSignedBig{ gTag<toSignType>, *toAbs };
    }
    return std::nullopt;
  }
}

} // namespace big
