#pragma once

// IWYU pragma: private, include <restricted.hpp>

#include <restricted/forward.hpp>
#include <signed.hpp>

namespace big {

template<IsSignedBig auto lb, IsSignedBig auto ub>
struct RestrictedBig
{
  using Traits   = impl::IsRestrictedBigHelper<lb, ub>;
  using Storage  = typename Traits::Storage;
  using Absolute = SignedBig<Traits::gSignType, Traits::gMagBits>;

  constexpr static auto gLb       = lb;
  constexpr static auto gUb       = ub;
  constexpr static auto gSignType = Traits::gSignType;
  constexpr static auto gMode     = Traits::gMode;
  constexpr static auto gBits     = Storage::gBits;

  [[no_unique_address]] Storage mStorage{};

  constexpr RestrictedBig()                     = default;
  constexpr ~RestrictedBig()                    = default;
  constexpr RestrictedBig(RestrictedBig&&)      = default;
  constexpr RestrictedBig(RestrictedBig const&) = default;

  constexpr auto operator=(RestrictedBig&&) -> RestrictedBig&      = default;
  constexpr auto operator=(RestrictedBig const&) -> RestrictedBig& = default;

  template<impl::IsRestrictedBigRhs<lb, ub> Value>
    requires(!Is<Value, RestrictedBig<lb, ub>>)
  constexpr explicit RestrictedBig(Value&& value);

  template<impl::IsRestrictedBigRhs<lb, ub> Value>
    requires(!Is<Value, RestrictedBig<lb, ub>>)
  constexpr auto operator=(Value&& value) -> RestrictedBig&;

  template<impl::IsRestrictedBigConstevalRhs<lb, ub> Value>
    requires(!Is<Value, RestrictedBig<lb, ub>>)
  consteval explicit RestrictedBig(Value&& value);

  template<impl::IsRestrictedBigConstevalRhs<lb, ub> Value>
    requires(!Is<Value, RestrictedBig<lb, ub>>)
  consteval auto operator=(Value&& value) -> RestrictedBig&;

  [[nodiscard]] constexpr auto absolute() const -> Absolute;
  [[nodiscard]] constexpr auto sign() const -> bool;
  constexpr static auto        min() -> RestrictedBig;
  constexpr static auto        max() -> RestrictedBig;
};

RestrictedBig() -> RestrictedBig<gSignedBig<0>, gSignedBig<0>>;

template<IsRestrictedBig Value>
RestrictedBig(Value const&) -> RestrictedBig<Value::gLb, Value::gUb>;

template<typename Value>
  requires(impl::TypeBoundHelper<std::remove_cvref_t<Value>>::value && !IsRestrictedBig<Value>)
RestrictedBig(Value const&) -> RestrictedBig<impl::TypeBoundHelper<std::remove_cvref_t<Value>>::gLb, impl::TypeBoundHelper<std::remove_cvref_t<Value>>::gUb>;

} // namespace big
