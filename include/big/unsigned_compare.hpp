#pragma once

#include <big/unsigned.hpp>             // big::UnsignedBig
#include <big/unsigned_type_traits.hpp> // big::IsUnsignedBig

#include <algorithm> // std::max, std::max
#include <compare>   // std::strong_ordering, std::is_eq

namespace big {

template<IsUnsignedBig Lhs,
         IsUnsignedBig Rhs>
constexpr auto
operator<=>(
  Lhs const& lhs,
  Rhs const& rhs) -> std::strong_ordering
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

template<IsUnsignedBig Lhs,
         IsUnsignedBig Rhs>
constexpr auto
operator==(
  Lhs const& lhs,
  Rhs const& rhs) -> bool
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

// clang-format off
#ifdef BIG_MAKE_WRAPPER
#error "BIG_MAKE_WRAPPER is already defined"
#endif
#define BIG_MAKE_WRAPPER(OP)                                                     \
  template<typename Rhs>                                                         \
    requires(!IsUnsignedBig<Rhs>) && requires(Rhs const& rhs){UnsignedBig{rhs};} \
  constexpr auto operator OP(IsUnsignedBig auto const& lhs, Rhs const& rhs)      \
  {                                                                              \
    return lhs OP UnsignedBig{ rhs };                                            \
  }                                                                              \
                                                                                 \
  template<typename Lhs>                                                         \
    requires(!IsUnsignedBig<Lhs>) && requires(Lhs const& lhs){UnsignedBig{lhs};} \
  constexpr auto operator OP(Lhs const& lhs, IsUnsignedBig auto const& rhs)      \
  {                                                                              \
    return UnsignedBig{ lhs } OP rhs;                                            \
  }
BIG_MAKE_WRAPPER(<=>)
BIG_MAKE_WRAPPER(==)
#undef BIG_MAKE_WRAPPER
// clang-format on

} // namespace big
