#pragma once

// IWYU pragma: private, include <signed.hpp>

#include <signed/declaration.hpp>
#include <signed/forward.hpp>
#include <signed/implementation/arithmetic.hpp>

#include <common.hpp>
#include <unsigned.hpp>

#include <compare>

namespace big {

template<IsSignedBig Lhs, IsSignedBig Rhs>
constexpr auto
operator<=>(Lhs const& lhs, Rhs const& rhs) -> std::strong_ordering
{
  constexpr auto zero = UnsignedBig<1>{};

  auto const lhsZero = lhs.abs() == zero;
  auto const rhsZero = rhs.abs() == zero;

  // +0 and -0 compare equal, the sign only matters for non-zero magnitudes
  if (lhsZero && rhsZero) {
    return std::strong_ordering::equal;
  }
  if (lhsZero) {
    return rhs.sign() ? std::strong_ordering::less : std::strong_ordering::greater;
  }
  if (rhsZero) {
    return lhs.sign() ? std::strong_ordering::greater : std::strong_ordering::less;
  }

  if (lhs.sign() != rhs.sign()) {
    return lhs.sign() ? std::strong_ordering::greater : std::strong_ordering::less;
  }

  // same sign: compare magnitudes, reversed for negative values
  return lhs.sign() ? lhs.abs() <=> rhs.abs() : rhs.abs() <=> lhs.abs();
}

// !=, <, >, <=, >= are derived from <=> and == by operator rewriting
template<SignType lhsSignType, Size lhsBits, SignType rhsSignType, Size rhsBits>
constexpr auto
operator==(SignedBig<lhsSignType, lhsBits> const& lhs, SignedBig<rhsSignType, rhsBits> const& rhs) -> bool
{
  return std::is_eq(lhs <=> rhs);
}

template<typename Rhs>
  requires(!IsSignedBig<Rhs>) && requires(Rhs const& rhs) { SignedBig{ rhs }; }
constexpr auto
operator<=>(IsSignedBig auto const& lhs, Rhs const& rhs)
{
  return lhs <=> SignedBig{ rhs };
}

template<typename Lhs>
  requires(!IsSignedBig<Lhs>) && requires(Lhs const& lhs) { SignedBig{ lhs }; }
constexpr auto
operator<=>(Lhs const& lhs, IsSignedBig auto const& rhs)
{
  return SignedBig{ lhs } <=> rhs;
}

template<typename Rhs>
  requires(!IsSignedBig<Rhs>) && requires(Rhs const& rhs) { SignedBig{ rhs }; }
constexpr auto
operator==(IsSignedBig auto const& lhs, Rhs const& rhs)
{
  return lhs == SignedBig{ rhs };
}

template<typename Lhs>
  requires(!IsSignedBig<Lhs>) && requires(Lhs const& lhs) { SignedBig{ lhs }; }
constexpr auto
operator==(Lhs const& lhs, IsSignedBig auto const& rhs)
{
  return SignedBig{ lhs } == rhs;
}

} // namespace big
