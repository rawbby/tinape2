#pragma once

#include <big/common.hpp>               // big::SignType
#include <big/signed.hpp>               // big::SignedBig
#include <big/unsigned_type_traits.hpp> // big::CommonUnsignedBig

namespace big {

template<SignType signType,
         Size     bits>
constexpr auto
operator-(
  GenericSigned<signType,
                bits> const& value)
{
  if constexpr (SignType::Positive == signType) {
    return GenericSigned<SignType::Negative, bits>{ value.abs() };
  } else if constexpr (SignType::Negative == signType) {
    return GenericSigned<SignType::Positive, bits>{ value.abs() };
  } else { // Positive == sign
    return GenericSigned<SignType::Runtime, bits>{ value.abs(), !value.sign() };
  }
}

template<SignType lhsSignType,
         Size     lhsBits,
         SignType rhsSignType,
         Size     rhsBits>
constexpr auto
operator+(
  GenericSigned<lhsSignType,
                lhsBits> const& lhs,
  GenericSigned<rhsSignType,
                rhsBits> const& rhs)
{
  // if compile time sign equality
  if constexpr (SignType::Runtime != lhsSignType && lhsSignType == rhsSignType) {
    return GenericSigned{ lhsSignType, lhs.abs() + rhs.abs() };

  } else { // all other cases require runtime evaluation

    // ensure compile time known return type that fits all return paths
    using SignedBigT = GenericSigned<SignType::Runtime, decltype(lhs.abs() + rhs.abs())::gBits>;

    // positive + positive || negative + negative
    if (lhs.sign() == rhs.sign()) {
      return SignedBigT{ lhs.abs() + rhs.abs(), lhs.sign() };
    }

    // For different signs we need to calculate
    // the absolute difference and find derive
    // the correct sign. Notice that UnsignedT
    // will always have more bits than needed
    // for eighter subtraction.

    constexpr bool lhsCTSign = lhsSignType != SignType::Runtime;

    // small-positive + big-negative || small-negative + big-positive
    if (lhs.abs() < rhs.abs()) {
      return SignedBigT{ rhs.abs() - lhs.abs(), lhsCTSign ? !lhs.sign() : rhs.sign() };
    }

    // big-positive + small-negative || big-negative + small-positive
    return SignedBigT{ lhs.abs() - rhs.abs(), lhsCTSign ? lhs.sign() : !rhs.sign() };
  }
}

template<SignType lhsSignType,
         Size     lhsBits,
         SignType rhsSignType,
         Size     rhsBits>
constexpr auto
operator-(
  GenericSigned<lhsSignType,
                lhsBits> const& lhs,
  GenericSigned<rhsSignType,
                rhsBits> const& rhs)
{
  // if compile time sign inequality
  if constexpr (SignType::Runtime != lhsSignType && SignType::Runtime != rhsSignType && lhsSignType != rhsSignType) {
    return GenericSigned{ lhsSignType, lhs.abs() + rhs.abs() };

  } else { // all other cases require runtime evaluation

    // ensure compile time known return type that fits all return paths
    using SignedBigT = GenericSigned<SignType::Runtime, decltype(lhs.abs() + rhs.abs())::gBits>;

    // positive - negative || negative - positive
    if (lhs.sign() != rhs.sign()) {
      return SignedBigT{ lhs.abs() + rhs.abs(), lhs.sign() };
    }

    // For same signs we need to calculate
    // the absolute difference and derive
    // the correct sign. Notice that UnsignedT
    // will always have more bits than needed
    // for either subtraction.

    constexpr bool lhsCTSign = lhsSignType != SignType::Runtime;

    // small-positive - big-positive || small-negative - big-negative
    if (lhs.abs() < rhs.abs()) {
      return SignedBigT{ rhs.abs() - lhs.abs(), lhsCTSign ? !lhs.sign() : !rhs.sign() };
    }

    // big-positive - small-positive || big-negative - small-negative
    return SignedBigT{ lhs.abs() - rhs.abs(), lhsCTSign ? lhs.sign() : rhs.sign() };
  }
}

template<SignType lhsSignType,
         Size     lhsBits,
         SignType rhsSignType,
         Size     rhsBits>
constexpr auto
operator*(
  GenericSigned<lhsSignType,
                lhsBits> const& lhs,
  GenericSigned<rhsSignType,
                rhsBits> const& rhs)
{
  if constexpr (SignType::Runtime == lhsSignType || SignType::Runtime == rhsSignType) {
    return GenericSigned{ SignType::Runtime, lhs.abs() * rhs.abs(), lhs.sign() == rhs.sign() };

  } else if constexpr (rhsSignType == lhsSignType) {
    return GenericSigned{ SignType::Positive, lhs.abs() * rhs.abs() };

  } else { // rhsSignType != lhsSignType
    return GenericSigned{ SignType::Negative, lhs.abs() * rhs.abs() };
  }
}
} // namespace big
