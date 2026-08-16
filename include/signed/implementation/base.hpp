#pragma once

// IWYU pragma: private, include <signed.hpp>

#include <signed/declaration.hpp>

namespace big {

template<SignType signType, Size bits>
constexpr auto
SignedBig<signType, bits>::abs() const -> UnsignedBig<bits> const&
{
  return std::get<0>(mUnderlying);
}

template<SignType signType, Size bits>
constexpr auto
SignedBig<signType, bits>::sign() const -> bool
{
  if constexpr (signType == Runtime) {
    return std::get<1>(mUnderlying);
  }
  return signType == Positive;
}

template<SignType signType, Size bits>
template<typename Value>
  requires(!IsSignedIntegral<Value>)
constexpr void
SignedBig<signType, bits>::set(Value const& value)
{
  set(value, signType != Negative);
}

template<SignType signType, Size bits>
template<IsSignedIntegral Value>
constexpr void
SignedBig<signType, bits>::set(Value value)
{
  using ValueUnsigned  = MakeUnsigned<Value>;
  auto const valueSign = value >= 0;
  auto const valueAbs  = static_cast<ValueUnsigned>(valueSign ? value : -value);
  set(valueAbs, valueSign);
}

template<SignType signType, Size bits>
template<typename Value>
constexpr void
SignedBig<signType, bits>::set(Value const& value, bool sign)
{
  if (std::is_constant_evaluated()) {
    if constexpr (signType == Positive) {
      if (!sign) {
        throw std::invalid_argument{ "positive sign type cannot hold negative values" };
      }
    } else if constexpr (signType == Negative) {
      if (sign) {
        throw std::invalid_argument{ "negative sign type cannot hold positive values" };
      }
    }
  }

  if constexpr (signType == Runtime) {
    std::get<1>(mUnderlying) = sign;
  }
  std::get<0>(mUnderlying) = value;
}

} // namespace big
