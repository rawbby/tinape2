#pragma once

// IWYU pragma: private, include <restricted.hpp>

#include <restricted/declaration.hpp>
#include <restricted/forward.hpp>

#include <common.hpp>
#include <signed.hpp>
#include <unsigned.hpp>

#include <compare>

namespace big {

template<IsRestrictedBig Lhs, IsRestrictedBig Rhs>
constexpr auto
operator<=>(Lhs const& lhs, Rhs const& rhs) -> std::strong_ordering
{
  if constexpr (Lhs::gUb < Rhs::gLb) {
    return std::strong_ordering::less;
  } else if constexpr (Lhs::gLb > Rhs::gUb) {
    return std::strong_ordering::greater;
  } else if constexpr (Lhs::gLb == Rhs::gLb && Lhs::gUb == Rhs::gUb) {
    return lhs.mStorage <=> rhs.mStorage;
  } else {
    return lhs.absolute() <=> rhs.absolute();
  }
}

template<IsRestrictedBig Lhs, IsRestrictedBig Rhs>
constexpr auto
operator==(Lhs const& lhs, Rhs const& rhs) -> bool
{
  if constexpr (Lhs::gUb < Rhs::gLb || Lhs::gLb > Rhs::gUb) {
    return false;
  } else if constexpr (Lhs::gLb == Rhs::gLb && Lhs::gUb == Rhs::gUb) {
    return lhs.mStorage == rhs.mStorage;
  } else {
    return lhs.absolute() == rhs.absolute();
  }
}

template<typename Rhs>
  requires(!IsRestrictedBig<Rhs>) && requires(Rhs const& rhs) { RestrictedBig{ rhs }; }
constexpr auto
operator<=>(IsRestrictedBig auto const& lhs, Rhs const& rhs)
{
  return lhs <=> RestrictedBig{ rhs };
}

template<typename Lhs>
  requires(!IsRestrictedBig<Lhs>) && requires(Lhs const& lhs) { RestrictedBig{ lhs }; }
constexpr auto
operator<=>(Lhs const& lhs, IsRestrictedBig auto const& rhs)
{
  return RestrictedBig{ lhs } <=> rhs;
}

template<typename Rhs>
  requires(!IsRestrictedBig<Rhs>) && requires(Rhs const& rhs) { RestrictedBig{ rhs }; }
constexpr auto
operator==(IsRestrictedBig auto const& lhs, Rhs const& rhs)
{
  return lhs == RestrictedBig{ rhs };
}

template<typename Lhs>
  requires(!IsRestrictedBig<Lhs>) && requires(Lhs const& lhs) { RestrictedBig{ lhs }; }
constexpr auto
operator==(Lhs const& lhs, IsRestrictedBig auto const& rhs)
{
  return RestrictedBig{ lhs } == rhs;
}

} // namespace big
