#pragma once

#include <big/common.hpp>   // big::SignType
#include <big/signed.hpp>   // big::SignedBig
#include <big/unsigned.hpp> // big::UnsignedBig

#include <compare> // std::strong_ordering, std::is_eq
#include <cstddef> // std::size_t

namespace big {

template<IsSignedBig Lhs,
         IsSignedBig Rhs>
constexpr auto
operator<=>(
  Lhs const& lhs,
  Rhs const& rhs) -> std::strong_ordering
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
template<SignType lhsSignType,
         Size     lhsBits,
         SignType rhsSignType,
         Size     rhsBits>
constexpr auto
operator==(
  GenericSigned<lhsSignType,
                lhsBits> const& lhs,
  GenericSigned<rhsSignType,
                rhsBits> const& rhs) -> bool
{
  return std::is_eq(lhs <=> rhs);
}

} // namespace big
