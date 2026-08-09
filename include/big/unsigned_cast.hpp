#pragma once

#include <big/common.hpp>               // big::BigElement, big::gBigElementBits
#include <big/unsigned.hpp>             // big::UnsignedBig
#include <big/unsigned_type_traits.hpp> // big::IsUnsignedBig

#include <algorithm> // std::min
#include <cstddef>   // std::size_t
#include <optional>  // std::optional

namespace big {

// truncation based cast without extensive
// runtime checking but simple (exact) masking
template<IsUnsignedBig ToUnsignedBig, IsUnsignedBig FromUnsignedBig>
[[nodiscard]] constexpr auto
castBigUnsafe(FromUnsignedBig const& value) -> ToUnsignedBig
{
  if (FromUnsignedBig::gNumElements == 0 || ToUnsignedBig::gNumElements == 0) {
    return ToUnsignedBig{};
  }

  constexpr auto backIndex = std::min(ToUnsignedBig::gNumElements, FromUnsignedBig::gNumElements) - 1;
  constexpr auto maskBits  = ToUnsignedBig::gBits % gElementBits;
  constexpr auto mask      = maskBits == 0 ? ~Element{ 0 } : (Element{ 1 } << maskBits) - 1;

  auto result = ToUnsignedBig{};
  for (Size i = 0; i < backIndex; ++i) {
    result.element(i, value.element(i));
  }
  result.element(backIndex, value.element(backIndex) & mask);
  return result;
}

// safe casting returns a valid value if fromValue
// fits toValue or std::nullopt
template<IsUnsignedBig ToUnsignedBig, IsUnsignedBig FromUnsignedBig>
[[nodiscard]] constexpr auto
castBigSafe(FromUnsignedBig const& value) -> std::optional<ToUnsignedBig>
{
  if (ToUnsignedBig::gBits >= FromUnsignedBig::gBits) {
    return castBigUnsafe<ToUnsignedBig>(value);
  }

  if (ToUnsignedBig::gNumElements == 0) {
    for (Size i = 0; i < FromUnsignedBig::gNumElements; ++i) {
      if (value.element(i) != 0) {
        return std::nullopt;
      }
    }
    return ToUnsignedBig{};
  }

  // FromUnsignedBig::gNumElements > ToUnsignedBig::gNumElements > 0

  constexpr auto    endIndex  = std::min(ToUnsignedBig::gNumElements, FromUnsignedBig::gNumElements);
  constexpr auto    backIndex = endIndex - 1;
  constexpr auto    maskBits  = ToUnsignedBig::gBits % gElementBits;
  constexpr Element mask      = maskBits == 0 ? ~Element{ 0 } : (Element{ 1 } << maskBits) - 1;

  for (Size i = FromUnsignedBig::gNumElements; i > endIndex;) {
    --i;
    if (value.element(i) != 0) {
      return std::nullopt;
    }
  }
  if ((value.element(backIndex) & ~mask) != 0) {
    return std::nullopt;
  }

  auto result = ToUnsignedBig{};
  result.element(backIndex, value.element(backIndex) & mask);

  if (backIndex != 0) {
    for (Size i = backIndex; i > 0;) {
      --i;
      result.element(i, value.element(i));
    }
  }

  return result;
}

} // namespace big
