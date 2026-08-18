#pragma once

// IWYU pragma: private, include <signed.hpp>

#include <signed/declaration.hpp>
#include <signed/forward.hpp>

#include <common.hpp>
#include <unsigned.hpp>

#include <type_traits>

namespace big {
namespace impl {

template<typename Value, SignType signType, Size bits>
constexpr void
set(SignedBig<signType, bits>& self, Value const& value, bool sign)
{
  if (std::is_constant_evaluated()) {
    if constexpr (signType != Runtime && IsSignedIntegral<Value>) {
      if (value < 0) {
        throw std::invalid_argument{ "non runtime signed big assumes positive parameter value" };
      }
    }
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
    self.mUnderlying.mSign = sign;
  }

  if constexpr (IsSignedIntegral<Value>) {
    set<false>(self.mUnderlying.mAbs, makeUnsigned(value));
  } else {
    set<false>(self.mUnderlying.mAbs, value);
  }
}

template<IsSignedIntegral Value, SignType signType, Size bits>
constexpr void
set(SignedBig<signType, bits>& self, Value value)
{
  using ValueUnsigned  = MakeUnsigned<Value>;
  auto const valueSign = value >= 0;
  auto const valueAbs  = static_cast<ValueUnsigned>(valueSign ? value : -value);

  if constexpr (signType == Runtime) {
    set(self, valueAbs, valueSign);
  } else {
    // for compile time sign types the magnitude is taken from the value and the sign from the type
    if (std::is_constant_evaluated() && !valueSign) {
      throw std::invalid_argument{ "non runtime signed big assumes positive parameter value" };
    }
    set(self, valueAbs, signType != Negative);
  }
}

template<IsSignedBig Value, SignType signType, Size bits>
constexpr void
set(SignedBig<signType, bits>& self, Value const& value)
{
  // a signed big carries its own sign which must be preserved instead of being taken from the target type
  set(self, value.abs(), value.sign());
}

template<typename Value, SignType signType, Size bits>
  requires(!IsSignedIntegral<Value> && !IsSignedBig<Value>)
constexpr void
set(SignedBig<signType, bits>& self, Value const& value)
{
  set(self, value, signType != Negative);
}

} // namespace impl

template<SignType signType, Size bits>
template<impl::IsSignedBigRhs<signType, bits> Value>
  requires(!Is<Value, SignedBig<signType, bits>>)
constexpr SignedBig<signType, bits>::SignedBig(Value&& value)
{
  impl::set(*this, std::forward<Value>(value));
}

template<SignType signType, Size bits>
template<impl::IsSignedBigRhs<signType, bits> Value>
constexpr SignedBig<signType, bits>::SignedBig(Tag<signType> /*tag*/, Value&& value)
{
  impl::set(*this, std::forward<Value>(value));
}

template<SignType signType, Size bits>
template<impl::IsSignedBigRhs<signType, bits> Value>
  requires(signType == Runtime)
constexpr SignedBig<signType, bits>::SignedBig(Value&& value, bool sign)
{
  impl::set(*this, std::forward<Value>(value), sign);
}

template<SignType signType, Size bits>
template<impl::IsSignedBigRhs<signType, bits> Value>
  requires(!Is<Value, SignedBig<signType, bits>>)
constexpr auto
SignedBig<signType, bits>::operator=(Value&& value) -> SignedBig&
{
  impl::set(*this, std::forward<Value>(value));
  return *this;
}

template<SignType signType, Size bits>
template<impl::IsSignedBigConstevalRhs<signType, bits> Value>
  requires(!Is<Value, SignedBig<signType, bits>>)
consteval SignedBig<signType, bits>::SignedBig(Value&& value)
{
  impl::set(*this, std::forward<Value>(value));
}

template<SignType signType, Size bits>
template<impl::IsSignedBigConstevalRhs<signType, bits> Value>
consteval SignedBig<signType, bits>::SignedBig(Tag<signType> /*tag*/, Value&& value)
{
  impl::set(*this, std::forward<Value>(value));
}

template<SignType signType, Size bits>
template<impl::IsSignedBigConstevalRhs<signType, bits> Value>
  requires(signType == Runtime)
consteval SignedBig<signType, bits>::SignedBig(Value&& value, bool sign)
{
  impl::set(*this, std::forward<Value>(value), sign);
}

template<SignType signType, Size bits>
template<impl::IsSignedBigConstevalRhs<signType, bits> Value>
  requires(!Is<Value, SignedBig<signType, bits>>)
consteval auto
SignedBig<signType, bits>::operator=(Value&& value) -> SignedBig&
{
  impl::set(*this, std::forward<Value>(value));
  return *this;
}

template<SignType signType, Size bits>
constexpr auto
SignedBig<signType, bits>::abs() const -> UnsignedBig<bits> const&
{
  return mUnderlying.mAbs;
}

template<SignType signType, Size bits>
constexpr auto
SignedBig<signType, bits>::sign() const -> bool
{
  return mUnderlying.mSign;
}

} // namespace big
