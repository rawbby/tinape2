#pragma once

// IWYU pragma: private, include <restricted.hpp>

#include <common.hpp>
#include <signed.hpp>
#include <unsigned.hpp>

#include <type_traits>

namespace big {

// NOLINTNEXTLINE(*-use-enum-class)
enum RestrictedMode : I8
{
  Naive        = 0,
  ExactShift   = 1,
  ElementShift = 2,
};

template<IsSignedBig auto lb, IsSignedBig auto ub>
struct RestrictedBig;

namespace impl {

template<IsSignedBig auto bound>
constexpr auto gElementFloor = [] {
  if constexpr (bound == 0) {
    return gSignedBig<0>;
  } else {
    constexpr auto lowVal = bound.abs().element(0);
    if constexpr (bound > 0) {
      return gSignedBig<bound - lowVal>;
    } else { // bound < 0
      if constexpr (lowVal == 0) {
        return gSignedBig<bound>;
      } else {
        constexpr auto rem = static_cast<Element>(Element{ 0 } - lowVal);
        return gSignedBig<bound - rem>;
      }
    }
  }
}();

template<IsSignedBig auto lb, IsSignedBig auto ub>
struct IsRestrictedBigHelper final : std::true_type
{
  static_assert(lb <= ub, "restricted lower bound must not exceed upper bound");

  constexpr static auto gLb = lb;
  constexpr static auto gUb = ub;

  constexpr static auto gSignType = [] {
    if constexpr (lb >= 0) {
      return SignType::Positive;
    } else if constexpr (ub <= 0) {
      return SignType::Negative;
    } else {
      return SignType::Runtime;
    }
  }();

  constexpr static auto gSpan     = ub - lb;
  constexpr static auto gSpanBits = decltype(gUnsignedBig<gSpan.abs()>)::gBits;
  constexpr static auto gMagBits  = decltype(gUnsignedBig<gMax<lb.abs(), ub.abs()>>)::gBits;

  constexpr static auto gMode = [] {
    if constexpr (gMagBits <= gWideBits) {
      return RestrictedMode::Naive;
    } else if constexpr (lb.sign() == ub.sign() && gSpanBits <= gWideBits) {
      return RestrictedMode::ExactShift;
    } else {
      return RestrictedMode::ElementShift;
    }
  }();

  constexpr static auto gBias = [] {
    if constexpr (gMode == RestrictedMode::Naive) {
      return gSignedBig<0>;
    } else if constexpr (gMode == RestrictedMode::ExactShift) {
      return lb;
    } else {
      return gElementFloor<lb>;
    }
  }();

  static_assert(gMode == RestrictedMode::Naive || lb - gBias >= 0, "restricted lower bound must not be smaller than bias");

  using Storage = std::conditional_t<gMode == RestrictedMode::Naive,
                                     SignedBig<gSignType, gMagBits>,
                                     std::conditional_t<gMode == RestrictedMode::ExactShift,
                                                        UnsignedBig<gSpanBits>,
                                                        UnsignedBig<((decltype(gUnsignedBig<(ub - gBias).abs()>)::gBits + gElementBits - 1) / gElementBits) * gElementBits>>>;

  constexpr static auto gBits = Storage::gBits;
};

template<typename T>
struct IsRestrictedBigType final : std::false_type
{};

template<IsSignedBig auto lb, IsSignedBig auto ub>
struct IsRestrictedBigType<RestrictedBig<lb, ub>> final : std::true_type
{
  using Helper              = IsRestrictedBigHelper<lb, ub>;
  constexpr static auto gLb = lb;
  constexpr static auto gUb = ub;
};

} // namespace impl

template<typename T>
concept IsRestrictedBig = impl::IsRestrictedBigType<std::remove_cvref_t<T>>::value;

template<IsRestrictedBig... Ts>
using CommonRestrictedBig = RestrictedBig<gMin<impl::IsRestrictedBigType<std::remove_cvref_t<Ts>>::gLb...>, gMax<impl::IsRestrictedBigType<std::remove_cvref_t<Ts>>::gUb...>>;

namespace impl {

template<typename T>
struct TypeBoundHelper final : std::false_type
{};

template<IsSignedBig auto lb, IsSignedBig auto ub>
struct TypeBoundHelper<RestrictedBig<lb, ub>> final : std::true_type
{
  constexpr static auto gLb = lb;
  constexpr static auto gUb = ub;
};

template<SignType signType, Size bits>
struct TypeBoundHelper<SignedBig<signType, bits>> final : std::true_type
{
  constexpr static auto gLb = [] {
    if constexpr (signType == SignType::Positive || bits == 0) {
      return gSignedBig<0>;
    } else {
      return SignedBig<SignType::Negative, bits>{ gTag<SignType::Negative>, UnsignedBig<bits>::max() };
    }
  }();
  constexpr static auto gUb = [] {
    if constexpr (signType == SignType::Negative || bits == 0) {
      return gSignedBig<0>;
    } else {
      return SignedBig<SignType::Positive, bits>{ gTag<SignType::Positive>, UnsignedBig<bits>::max() };
    }
  }();
};

template<Size bits>
struct TypeBoundHelper<UnsignedBig<bits>> final : std::true_type
{
  constexpr static auto gLb = gSignedBig<0>;
  constexpr static auto gUb = [] {
    if constexpr (bits == 0) {
      return gSignedBig<0>;
    } else {
      return SignedBig<SignType::Positive, bits>{ gTag<SignType::Positive>, UnsignedBig<bits>::max() };
    }
  }();
};

template<Size size>
struct TypeBoundHelper<Elements<size>> final : std::true_type
{
  constexpr static auto gBits = size * gElementBits;
  constexpr static auto gLb   = gSignedBig<0>;
  constexpr static auto gUb   = SignedBig<SignType::Positive, gBits>{ gTag<SignType::Positive>, UnsignedBig<gBits>::max() };
};

template<Size size>
  requires(!gLimbIsElement)
struct TypeBoundHelper<Limbs<size>> final : std::true_type
{
  constexpr static auto gBits = size * gLimbBits;
  constexpr static auto gLb   = gSignedBig<0>;
  constexpr static auto gUb   = SignedBig<SignType::Positive, gBits>{ gTag<SignType::Positive>, UnsignedBig<gBits>::max() };
};

template<IsUnsignedIntegral T>
struct TypeBoundHelper<T> final : std::true_type
{
  using CleanT                = std::remove_cvref_t<T>;
  constexpr static auto gBits = gBitsOf<CleanT>;
  constexpr static auto gLb   = gSignedBig<0>;
  constexpr static auto gUb   = SignedBig<SignType::Positive, gBits>{ gTag<SignType::Positive>, UnsignedBig<gBits>::max() };
};

template<IsSignedIntegral T>
struct TypeBoundHelper<T> final : std::true_type
{
  using CleanT                = std::remove_cvref_t<T>;
  constexpr static auto gBits = gBitsOf<CleanT> - 1;
  constexpr static auto gLb   = SignedBig<SignType::Negative, gBits>{ gTag<SignType::Negative>, UnsignedBig<gBits>::max() };
  constexpr static auto gUb   = SignedBig<SignType::Positive, gBits>{ gTag<SignType::Positive>, UnsignedBig<gBits>::max() };
};

/// concept for all runtime initializers to restricted big
template<typename Value, auto lb, auto ub>
concept IsRestrictedBigRhs =
  TypeBoundHelper<std::remove_cvref_t<Value>>::value && TypeBoundHelper<std::remove_cvref_t<Value>>::gLb >= lb && TypeBoundHelper<std::remove_cvref_t<Value>>::gUb <= ub;

/// concept for all consteval but not runtime initializers to restricted big
template<typename Value, auto lb, auto ub>
concept IsRestrictedBigConstevalRhs = !IsRestrictedBigRhs<Value, lb, ub> && TypeBoundHelper<std::remove_cvref_t<Value>>::value;

} // namespace impl

} // namespace big
