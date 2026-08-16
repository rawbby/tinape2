#pragma once

// IWYU pragma: private, include <signed.hpp>

#include <signed/forward.hpp>

#include <common.hpp>
#include <unsigned.hpp>

#include <type_traits>

namespace big {

template<SignType signType, Size bits>
class SignedBig
{
  using Traits = IsSignedBigHelper<SignedBig<signType, bits>>;

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

  template<IsSignedBigInit<signType, bits> Value>
    requires(!Is<Value, SignedBig<signType, bits>>)
  constexpr explicit SignedBig(Value&& value);

  template<IsSignedBigInit<signType, bits> Value>
    requires(gSignType == Runtime)
  constexpr explicit SignedBig(Value&& value, bool sign);

  template<IsSignedBigInit<signType, bits> Value>
    requires(!Is<Value, SignedBig<signType, bits>>)
  constexpr auto operator=(Value&& value) -> SignedBig&;

  template<IsSignedBigConstevalInit<signType, bits> Value>
    requires(!Is<Value, SignedBig<signType, bits>>)
  consteval explicit SignedBig(Value&& value);

  template<IsSignedBigConstevalInit<signType, bits> Value>
    requires(gSignType == Runtime)
  consteval explicit SignedBig(Value&& value, bool sign);

  template<IsSignedBigConstevalInit<signType, bits> Value>
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

template<IsUnsignedBig Value>
SignedBig(Value const&) -> SignedBig<Positive, Value::gBits>;

template<IsUnsignedBig Value>
SignedBig(Value const&, bool) -> SignedBig<Runtime, Value::gBits>;

template<IsUnsignedIntegral Value>
SignedBig(Value) -> SignedBig<Positive, gBitsOf<Value>>;

template<IsUnsignedIntegral Value>
SignedBig(Value, bool) -> SignedBig<Runtime, gBitsOf<Value>>;

template<IsElements Value>
SignedBig(Value const&) -> SignedBig<Positive, gBitsOf<Value>>;

template<IsElements Value>
SignedBig(Value const&, bool) -> SignedBig<Runtime, gBitsOf<Value>>;

template<IsLimbs Value>
  requires(!gLimbIsElement)
SignedBig(Value const&) -> SignedBig<Positive, gBitsOf<Value>>;

template<IsLimbs Value>
  requires(!gLimbIsElement)
SignedBig(Value const&, bool) -> SignedBig<Runtime, gBitsOf<Value>>;

} // namespace big
