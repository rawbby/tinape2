#pragma once

// IWYU pragma: private, include <restricted.hpp>

#include <restricted/declaration.hpp>
#include <restricted/forward.hpp>

#include <common.hpp>
#include <signed.hpp>
#include <unsigned.hpp>

#include <stdexcept>
#include <type_traits>

namespace big {
namespace impl {

template<IsSignedBig auto lb, IsSignedBig auto ub, typename Value>
constexpr void
set(RestrictedBig<lb, ub>& self, Value const& value)
{
  using Traits  = typename RestrictedBig<lb, ub>::Traits;
  using Storage = typename Traits::Storage;

  auto const signedVal = [&]() -> auto {
    if constexpr (IsRestrictedBig<Value>) {
      return value.absolute();
    } else {
      return SignedBig{ value };
    }
  }();

  if (std::is_constant_evaluated()) {
    if (signedVal < lb || signedVal > ub) {
      throw std::invalid_argument{ "value out of restricted bounds" };
    }
  }

  if constexpr (Traits::gMode == RestrictedMode::Naive) {
    impl::set(self.mStorage, signedVal);
  } else if constexpr (Traits::gMode == RestrictedMode::ExactShift) {
    if constexpr (lb >= 0) {
      self.mStorage = castBigUnsafe<Storage>(signedVal.abs() - lb.abs());
    } else {
      self.mStorage = castBigUnsafe<Storage>(lb.abs() - signedVal.abs());
    }
  } else { // ElementShift
    constexpr auto bias = Traits::gBias;
    if constexpr (bias >= 0) {
      self.mStorage = castBigUnsafe<Storage>(signedVal.abs() - bias.abs());
    } else {
      if (signedVal.sign()) {
        self.mStorage = castBigUnsafe<Storage>(signedVal.abs() + bias.abs());
      } else {
        self.mStorage = castBigUnsafe<Storage>(bias.abs() - signedVal.abs());
      }
    }
  }
}

} // namespace impl

template<IsSignedBig auto lb, IsSignedBig auto ub>
template<impl::IsRestrictedBigRhs<lb, ub> Value>
  requires(!Is<Value, RestrictedBig<lb, ub>>)
constexpr RestrictedBig<lb, ub>::RestrictedBig(Value&& value)
{
  impl::set(*this, std::forward<Value>(value));
}

template<IsSignedBig auto lb, IsSignedBig auto ub>
template<impl::IsRestrictedBigRhs<lb, ub> Value>
  requires(!Is<Value, RestrictedBig<lb, ub>>)
constexpr auto
RestrictedBig<lb, ub>::operator=(Value&& value) -> RestrictedBig&
{
  impl::set(*this, std::forward<Value>(value));
  return *this;
}

template<IsSignedBig auto lb, IsSignedBig auto ub>
template<impl::IsRestrictedBigConstevalRhs<lb, ub> Value>
  requires(!Is<Value, RestrictedBig<lb, ub>>)
consteval RestrictedBig<lb, ub>::RestrictedBig(Value&& value)
{
  impl::set(*this, std::forward<Value>(value));
}

template<IsSignedBig auto lb, IsSignedBig auto ub>
template<impl::IsRestrictedBigConstevalRhs<lb, ub> Value>
  requires(!Is<Value, RestrictedBig<lb, ub>>)
consteval auto
RestrictedBig<lb, ub>::operator=(Value&& value) -> RestrictedBig&
{
  impl::set(*this, std::forward<Value>(value));
  return *this;
}

template<IsSignedBig auto lb, IsSignedBig auto ub>
constexpr auto
RestrictedBig<lb, ub>::absolute() const -> Absolute
{
  if constexpr (gMode == RestrictedMode::Naive) {
    return mStorage;
  } else if constexpr (gMode == RestrictedMode::ExactShift) {
    if constexpr (lb >= 0) {
      return Absolute{ gTag<gSignType>, castBigUnsafe<UnsignedBig<Traits::gMagBits>>(lb.abs() + mStorage) };
    } else {
      return Absolute{ gTag<gSignType>, castBigUnsafe<UnsignedBig<Traits::gMagBits>>(lb.abs() - mStorage) };
    }
  } else { // ElementShift
    constexpr auto bias = Traits::gBias;
    if constexpr (bias >= 0) {
      return Absolute{ gTag<gSignType>, castBigUnsafe<UnsignedBig<Traits::gMagBits>>(bias.abs() + mStorage) };
    } else {
      if constexpr (gSignType == SignType::Negative) {
        return Absolute{ gTag<gSignType>, castBigUnsafe<UnsignedBig<Traits::gMagBits>>(bias.abs() - mStorage) };
      } else if constexpr (gSignType == SignType::Positive) {
        return Absolute{ gTag<gSignType>, castBigUnsafe<UnsignedBig<Traits::gMagBits>>(mStorage - bias.abs()) };
      } else { // Runtime
        if (mStorage >= bias.abs()) {
          return Absolute{ castBigUnsafe<UnsignedBig<Traits::gMagBits>>(mStorage - bias.abs()), true };
        }
        return Absolute{ castBigUnsafe<UnsignedBig<Traits::gMagBits>>(bias.abs() - mStorage), false };
      }
    }
  }
}

template<IsSignedBig auto lb, IsSignedBig auto ub>
constexpr auto
RestrictedBig<lb, ub>::sign() const -> bool
{
  if constexpr (gSignType == SignType::Positive) {
    return true;
  } else if constexpr (gSignType == SignType::Negative) {
    return false;
  } else if constexpr (gMode == RestrictedMode::Naive) {
    return mStorage.sign();
  } else {
    return absolute().sign();
  }
}

template<IsSignedBig auto lb, IsSignedBig auto ub>
constexpr auto
RestrictedBig<lb, ub>::min() -> RestrictedBig
{
  constexpr auto result = [] {
    RestrictedBig res{};
    impl::set(res, lb);
    return res;
  }();
  return result;
}

template<IsSignedBig auto lb, IsSignedBig auto ub>
constexpr auto
RestrictedBig<lb, ub>::max() -> RestrictedBig
{
  constexpr auto result = [] {
    RestrictedBig res{};
    impl::set(res, ub);
    return res;
  }();
  return result;
}

} // namespace big
