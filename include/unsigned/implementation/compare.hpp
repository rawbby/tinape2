#pragma once

// IWYU pragma: private, include <unsigned.hpp>

#include <unsigned/declaration.hpp>
#include <unsigned/forward.hpp>

#include <common/int.hpp>

#include <algorithm>
#include <compare>

namespace big {

template<IsUnsignedBig Lhs, IsUnsignedBig Rhs>
constexpr auto
operator<=>(Lhs const& lhs, Rhs const& rhs) -> std::strong_ordering
{
  constexpr auto max = std::max(Lhs::gNumElements, Rhs::gNumElements);
  constexpr auto min = std::min(Lhs::gNumElements, Rhs::gNumElements);

  auto index = max;

  if constexpr (min < Lhs::gNumElements) {
    while (index > min) {
      --index;
      if (lhs.element(index) != 0) {
        return std::strong_ordering::greater;
      }
    }
  }

  if constexpr (min < Rhs::gNumElements) {
    while (index > min) {
      --index;
      if (rhs.element(index) != 0) {
        return std::strong_ordering::less;
      }
    }
  }

  if constexpr (min > 0) {
    while (index > 0) {
      --index;
      auto const cmp = lhs.element(index) <=> rhs.element(index);
      if (cmp != std::strong_ordering::equal) {
        return cmp;
      }
    }
  }

  return std::strong_ordering::equal;
}

template<IsUnsignedBig Lhs, IsUnsignedBig Rhs>
constexpr auto
operator==(Lhs const& lhs, Rhs const& rhs) -> bool
{
  constexpr auto max = std::max(Lhs::gNumElements, Rhs::gNumElements);
  constexpr auto min = std::min(Lhs::gNumElements, Rhs::gNumElements);

  for (Size i = 0; i < min; ++i) {
    if (lhs.element(i) != rhs.element(i)) {
      return false;
    }
  }

  if constexpr (min < Lhs::gNumElements) {
    for (Size i = min; i < max; ++i) {
      if (lhs.element(i) != 0) {
        return false;
      }
    }
  }

  if constexpr (min < Rhs::gNumElements) {
    for (Size i = min; i < max; ++i) {
      if (rhs.element(i) != 0) {
        return false;
      }
    }
  }

  return true;
}

template<typename Rhs>
  requires(!IsUnsignedBig<Rhs>) && requires(Rhs const& rhs) { UnsignedBig{ rhs }; }
constexpr auto
operator<=>(IsUnsignedBig auto const& lhs, Rhs const& rhs)
{
  return lhs <=> UnsignedBig{ rhs };
}

template<typename Lhs>
  requires(!IsUnsignedBig<Lhs>) && requires(Lhs const& lhs) { UnsignedBig{ lhs }; }
constexpr auto
operator<=>(Lhs const& lhs, IsUnsignedBig auto const& rhs)
{
  return UnsignedBig{ lhs } <=> rhs;
}

template<typename Rhs>
  requires(!IsUnsignedBig<Rhs>) && requires(Rhs const& rhs) { UnsignedBig{ rhs }; }
constexpr auto
operator==(IsUnsignedBig auto const& lhs, Rhs const& rhs)
{
  return lhs == UnsignedBig{ rhs };
}

template<typename Lhs>
  requires(!IsUnsignedBig<Lhs>) && requires(Lhs const& lhs) { UnsignedBig{ lhs }; }
constexpr auto
operator==(Lhs const& lhs, IsUnsignedBig auto const& rhs)
{
  return UnsignedBig{ lhs } == rhs;
}

} // namespace big
