#pragma once

// IWYU pragma: private, include <signed.hpp>

#include <signed/declaration.hpp>
#include <signed/forward.hpp>

#include <common.hpp>

namespace big {

template<SignType signType, Size bits>
constexpr auto
operator-(SignedBig<signType, bits> const& value)
{
  if constexpr (SignType::Positive == signType) {
    return SignedBig<SignType::Negative, bits>{ value.abs() };
  } else if constexpr (SignType::Negative == signType) {
    return SignedBig<SignType::Positive, bits>{ value.abs() };
  } else { // Positive == sign
    return SignedBig<SignType::Runtime, bits>{ value.abs(), !value.sign() };
  }
}

template<SignType lhsSignType, Size lhsBits, SignType rhsSignType, Size rhsBits>
constexpr auto
operator+(SignedBig<lhsSignType, lhsBits> const& lhs, SignedBig<rhsSignType, rhsBits> const& rhs)
{
  // if compile time sign equality
  if constexpr (SignType::Runtime != lhsSignType && lhsSignType == rhsSignType) {
    return SignedBig{ lhsSignType, lhs.abs() + rhs.abs() };

  } else { // all other cases require runtime evaluation

    // ensure compile time known return type that fits all return paths
    using SignedBigT = SignedBig<SignType::Runtime, decltype(lhs.abs() + rhs.abs())::gBits>;

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

template<SignType lhsSignType, Size lhsBits, SignType rhsSignType, Size rhsBits>
constexpr auto
operator-(SignedBig<lhsSignType, lhsBits> const& lhs, SignedBig<rhsSignType, rhsBits> const& rhs)
{
  // if compile time sign inequality
  if constexpr (SignType::Runtime != lhsSignType && SignType::Runtime != rhsSignType && lhsSignType != rhsSignType) {
    return SignedBig{ lhsSignType, lhs.abs() + rhs.abs() };

  } else { // all other cases require runtime evaluation

    // ensure compile time known return type that fits all return paths
    using SignedBigT = SignedBig<SignType::Runtime, decltype(lhs.abs() + rhs.abs())::gBits>;

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

template<SignType lhsSignType, Size lhsBits, SignType rhsSignType, Size rhsBits>
constexpr auto
operator*(SignedBig<lhsSignType, lhsBits> const& lhs, SignedBig<rhsSignType, rhsBits> const& rhs)
{
  if constexpr (SignType::Runtime == lhsSignType || SignType::Runtime == rhsSignType) {
    return SignedBig{ SignType::Runtime, lhs.abs() * rhs.abs(), lhs.sign() == rhs.sign() };

  } else if constexpr (rhsSignType == lhsSignType) {
    return SignedBig{ SignType::Positive, lhs.abs() * rhs.abs() };

  } else { // rhsSignType != lhsSignType
    return SignedBig{ SignType::Negative, lhs.abs() * rhs.abs() };
  }
}

// NOLINTBEGIN(*-macro-usage)
// clang-format off
#ifdef BIG_MAKE_WRAPPER
#error "BIG_MAKE_WRAPPER is already defined"
#endif
#define BIG_MAKE_WRAPPER(OP)                                                 \
  template<typename Rhs>                                                     \
    requires(!IsSignedBig<Rhs>) && requires(Rhs const& rhs){SignedBig{rhs};} \
  constexpr auto operator OP(IsSignedBig auto const& lhs, Rhs const& rhs)    \
  {                                                                          \
    return lhs OP SignedBig{ rhs };                                          \
  }                                                                          \
                                                                             \
  template<typename Lhs>                                                     \
    requires(!IsSignedBig<Lhs>) && requires(Lhs const& lhs){SignedBig{lhs};} \
  constexpr auto operator OP(Lhs const& lhs, IsSignedBig auto const& rhs)    \
  {                                                                          \
    return SignedBig{ lhs } OP rhs;                                          \
  }
BIG_MAKE_WRAPPER(+)
BIG_MAKE_WRAPPER(-)
BIG_MAKE_WRAPPER(*)
#undef BIG_MAKE_WRAPPER
// clang-format on
// NOLINTEND(*-macro-usage)

} // namespace big
