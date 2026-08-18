#pragma once

// IWYU pragma: private, include <signed.hpp>

#include <signed/forward.hpp>

#include <common.hpp>
#include <unsigned.hpp>

#include <compare>
#include <optional>

namespace big {
template<SignType signType, Size bits>
class SignedBig
{
  using Traits = impl::IsSignedBigHelper<SignedBig<signType, bits>>;

public:
  using Unsigned   = Traits::Unsigned;
  using Underlying = Traits::Underlying;
  using Data       = Traits::Data;

  constexpr static auto gBits          = Traits::gBits;
  constexpr static auto gNumElements   = Traits::gNumElements;
  constexpr static auto gIsZeroData    = Traits::gIsZeroData;
  constexpr static auto gIsArrayData   = Traits::gIsArrayData;
  constexpr static auto gIsIntData     = Traits::gIsIntData;
  constexpr static auto gIsElementData = Traits::gIsElementData;
  constexpr static auto gIsWideData    = Traits::gIsWideData;
  constexpr static auto gIsNarrowData  = Traits::gIsNarrowData;
  constexpr static auto gSignType      = Traits::gSignType;

  [[no_unique_address]] Underlying mUnderlying{};

  constexpr SignedBig()                 = default;
  constexpr ~SignedBig()                = default;
  constexpr SignedBig(SignedBig&&)      = default;
  constexpr SignedBig(SignedBig const&) = default;

  constexpr auto operator=(SignedBig&&) -> SignedBig&      = default;
  constexpr auto operator=(SignedBig const&) -> SignedBig& = default;

  template<impl::IsSignedBigRhs<signType, bits> Value>
    requires(!Is<Value, SignedBig<signType, bits>>)
  constexpr explicit SignedBig(Value&& value);

  template<impl::IsSignedBigRhs<signType, bits> Value>
    requires(!Is<Value, SignedBig<signType, bits>>)
  constexpr explicit SignedBig(Tag<signType> /*tag*/, Value&& value);

  template<impl::IsSignedBigRhs<signType, bits> Value>
    requires(signType == Runtime)
  constexpr explicit SignedBig(Value&& value, bool sign);

  template<impl::IsSignedBigRhs<signType, bits> Value>
    requires(!Is<Value, SignedBig<signType, bits>>)
  constexpr auto operator=(Value&& value) -> SignedBig&;

  template<impl::IsSignedBigConstevalRhs<signType, bits> Value>
    requires(!Is<Value, SignedBig<signType, bits>>)
  consteval explicit SignedBig(Value&& value);

  template<impl::IsSignedBigConstevalRhs<signType, bits> Value>
    requires(!Is<Value, SignedBig<signType, bits>>)
  consteval explicit SignedBig(Tag<signType> /*tag*/, Value&& value);

  template<impl::IsSignedBigConstevalRhs<signType, bits> Value>
    requires(signType == Runtime)
  consteval explicit SignedBig(Value&& value, bool sign);

  template<impl::IsSignedBigConstevalRhs<signType, bits> Value>
    requires(!Is<Value, SignedBig<signType, bits>>)
  consteval auto operator=(Value&& value) -> SignedBig&;

  [[nodiscard]] constexpr auto abs() const -> UnsignedBig<bits> const&;

  [[nodiscard]] constexpr auto sign() const -> bool;

private:
  template<typename Value>
    requires(!IsSignedIntegral<Value>)
  constexpr void set(Value const& value);

  template<IsSignedIntegral Value>
  constexpr void set(Value value);

  template<typename Value>
  constexpr void set(Value const& value, bool sign);
};

SignedBig() -> SignedBig<Positive, 0>;

template<IsSignedBig Value>
SignedBig(Value const&) -> SignedBig<Value::gSignType, Value::gBits>;

template<IsSignedIntegral Value>
SignedBig(Value) -> SignedBig<Runtime, gBitsOf<Value> - 1>;

template<IsSignedIntegral Value, SignType signType>
SignedBig(Tag<signType>, Value) -> SignedBig<signType, gBitsOf<Value> - 1>;

template<IsUnsignedBig Value>
SignedBig(Value const&) -> SignedBig<Positive, Value::gBits>;

template<IsUnsignedBig Value, SignType signType>
SignedBig(Tag<signType>, Value const&) -> SignedBig<signType, Value::gBits>;

template<IsUnsignedBig Value>
SignedBig(Value const&, bool) -> SignedBig<Runtime, Value::gBits>;

template<IsUnsignedIntegral Value>
SignedBig(Value) -> SignedBig<Positive, gBitsOf<Value>>;

template<IsUnsignedIntegral Value, SignType signType>
SignedBig(Tag<signType>, Value) -> SignedBig<signType, gBitsOf<Value>>;

template<IsUnsignedIntegral Value>
SignedBig(Value, bool) -> SignedBig<Runtime, gBitsOf<Value>>;

template<IsElements Value>
SignedBig(Value const&) -> SignedBig<Positive, gBitsOf<Value>>;

template<IsElements Value, SignType signType>
SignedBig(Tag<signType>, Value const&) -> SignedBig<signType, gBitsOf<Value>>;

template<IsElements Value>
SignedBig(Value const&, bool) -> SignedBig<Runtime, gBitsOf<Value>>;

template<IsLimbs Value>
  requires(!gLimbIsElement)
SignedBig(Value const&) -> SignedBig<Positive, gBitsOf<Value>>;

template<IsLimbs Value, SignType signType>
  requires(!gLimbIsElement)
SignedBig(Tag<signType>, Value const&) -> SignedBig<signType, gBitsOf<Value>>;

template<IsLimbs Value>
  requires(!gLimbIsElement)
SignedBig(Value const&, bool) -> SignedBig<Runtime, gBitsOf<Value>>;

template<SignType signType, Size bits>
constexpr auto
operator-(SignedBig<signType, bits> const& value);

template<SignType lhsSignType, Size lhsBits, SignType rhsSignType, Size rhsBits>
constexpr auto
operator+(SignedBig<lhsSignType, lhsBits> const& lhs, SignedBig<rhsSignType, rhsBits> const& rhs);

template<SignType lhsSignType, Size lhsBits, SignType rhsSignType, Size rhsBits>
constexpr auto
operator-(SignedBig<lhsSignType, lhsBits> const& lhs, SignedBig<rhsSignType, rhsBits> const& rhs);

template<SignType lhsSignType, Size lhsBits, SignType rhsSignType, Size rhsBits>
constexpr auto
operator*(SignedBig<lhsSignType, lhsBits> const& lhs, SignedBig<rhsSignType, rhsBits> const& rhs);

template<typename Rhs>
  requires(!IsSignedBig<Rhs>) && requires(Rhs const& rhs) { SignedBig{ rhs }; }
constexpr auto
operator+(IsSignedBig auto const& lhs, Rhs const& rhs);

template<typename Lhs>
  requires(!IsSignedBig<Lhs>) && requires(Lhs const& lhs) { SignedBig{ lhs }; }
constexpr auto
operator+(Lhs const& lhs, IsSignedBig auto const& rhs);

template<typename Rhs>
  requires(!IsSignedBig<Rhs>) && requires(Rhs const& rhs) { SignedBig{ rhs }; }
constexpr auto
operator-(IsSignedBig auto const& lhs, Rhs const& rhs);

template<typename Lhs>
  requires(!IsSignedBig<Lhs>) && requires(Lhs const& lhs) { SignedBig{ lhs }; }
constexpr auto
operator-(Lhs const& lhs, IsSignedBig auto const& rhs);

template<typename Rhs>
  requires(!IsSignedBig<Rhs>) && requires(Rhs const& rhs) { SignedBig{ rhs }; }
constexpr auto
operator*(IsSignedBig auto const& lhs, Rhs const& rhs);

template<typename Lhs>
  requires(!IsSignedBig<Lhs>) && requires(Lhs const& lhs) { SignedBig{ lhs }; }
constexpr auto
operator*(Lhs const& lhs, IsSignedBig auto const& rhs);

template<IsSignedBig Lhs, IsSignedBig Rhs>
constexpr auto
operator<=>(Lhs const& lhs, Rhs const& rhs) -> std::strong_ordering;

template<SignType lhsSignType, Size lhsBits, SignType rhsSignType, Size rhsBits>
constexpr auto
operator==(SignedBig<lhsSignType, lhsBits> const& lhs, SignedBig<rhsSignType, rhsBits> const& rhs) -> bool;

template<typename Rhs>
  requires(!IsSignedBig<Rhs>) && requires(Rhs const& rhs) { SignedBig{ rhs }; }
constexpr auto
operator<=>(IsSignedBig auto const& lhs, Rhs const& rhs);

template<typename Lhs>
  requires(!IsSignedBig<Lhs>) && requires(Lhs const& lhs) { SignedBig{ lhs }; }
constexpr auto
operator<=>(Lhs const& lhs, IsSignedBig auto const& rhs);

template<typename Rhs>
  requires(!IsSignedBig<Rhs>) && requires(Rhs const& rhs) { SignedBig{ rhs }; }
constexpr auto
operator==(IsSignedBig auto const& lhs, Rhs const& rhs);

template<typename Lhs>
  requires(!IsSignedBig<Lhs>) && requires(Lhs const& lhs) { SignedBig{ lhs }; }
constexpr auto
operator==(Lhs const& lhs, IsSignedBig auto const& rhs);

/// truncation based cast without extensive runtime checking but simple (exact) masking
template<IsSignedBig ToSignedBig, SignType fromSignType, Size fromBits>
[[nodiscard]] constexpr auto
castBigUnsafe(SignedBig<fromSignType, fromBits> const& fromValue) -> ToSignedBig;

/// safe casting returns a valid value if fromValue fits toValue or std::nullopt
template<IsSignedBig ToSignedBig, SignType fromSignType, Size fromBits>
[[nodiscard]] constexpr auto
castBigSafe(SignedBig<fromSignType, fromBits> const& fromValue) -> std::optional<ToSignedBig>;

} // namespace big
