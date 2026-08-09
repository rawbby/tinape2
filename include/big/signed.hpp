#pragma once

#include <big/common.hpp>
#include <big/unsigned.hpp>
#include <common/int.hpp>

#include <tuple>
#include <type_traits>

namespace big {

template<SignType signType, Size bits>
struct SignedBig
{
  using Unsigned = UnsignedBig<bits>;
  using Data     = Unsigned::Data;

  constexpr static auto gBits          = Unsigned::gBits;
  constexpr static auto gNumElements   = Unsigned::gNumElements;
  constexpr static auto gIsZeroData    = Unsigned::gIsZeroData;
  constexpr static auto gIsArrayData   = Unsigned::gIsArrayData;
  constexpr static auto gIsIntData     = Unsigned::gIsIntData;
  constexpr static auto gIsElementData = Unsigned::gIsElementData;
  constexpr static auto gIsWideData    = Unsigned::gIsWideData;
  constexpr static auto gIsNarrowData  = Unsigned::gIsNarrowData;

  [[no_unique_address]] std::conditional_t<signType == Runtime, std::tuple<Unsigned, bool>, std::tuple<Unsigned>> mUnderlying{};

  constexpr SignedBig()                 = default;
  constexpr ~SignedBig()                = default;
  constexpr SignedBig(SignedBig&&)      = default;
  constexpr SignedBig(SignedBig const&) = default;

  constexpr auto operator=(SignedBig&&) -> SignedBig&      = default;
  constexpr auto operator=(SignedBig const&) -> SignedBig& = default;

  template<SignType otherSignType, Size otherBits>
    requires((signType == Runtime || signType == otherSignType) && otherBits <= bits)
  constexpr explicit SignedBig(SignedBig<otherSignType, otherBits> const& other)
  {
    set(other.abs().mData, other.sign());
  }

  template<SignType otherSignType, Size otherBits>
    requires((signType == Runtime || signType == otherSignType) && otherBits <= bits)
  constexpr auto operator=(SignedBig<otherSignType, otherBits> const& other) -> SignedBig&
  {
    set(other.abs().mData, other.sign());
    return *this;
  }

  template<Size otherBits>
    requires(otherBits <= bits)
  constexpr explicit SignedBig(UnsignedBig<otherBits> const& other)
  {
    set(other.mData);
  }

  template<Size otherBits>
    requires(otherBits <= bits)
  constexpr auto operator=(UnsignedBig<otherBits> const& other) -> SignedBig&
  {
    set(other.mData);
    return *this;
  }

  template<Size otherBits>
    requires(signType == Runtime && otherBits <= bits)
  constexpr SignedBig(UnsignedBig<otherBits> const& other, bool sign)
  {
    set(other.mData, sign);
  }

  template<IsUnsignedIntegral Value>
    requires(gBitsOf<Value> <= bits)
  constexpr explicit SignedBig(Value value)
  {
    set(value);
  }

  template<IsUnsignedIntegral Value>
    requires(gBitsOf<Value> <= bits)
  constexpr auto operator=(Value value) -> SignedBig&
  {
    set(value);
    return *this;
  }

  template<IsUnsignedIntegral Value>
    requires(signType == Runtime && gBitsOf<Value> <= bits)
  constexpr explicit SignedBig(Value value, bool sign)
  {
    set(value, sign);
  }

  template<IsSignedIntegral Value>
    requires(signType == Runtime && gBitsOf<Value> <= bits)
  constexpr explicit SignedBig(Value value)
  {
    set(value);
  }

  template<IsSignedIntegral Value>
    requires(signType == Runtime && gBitsOf<Value> <= bits)
  constexpr auto operator=(Value value) -> SignedBig&
  {
    set(value);
    return *this;
  }

  template<Size size>
    requires(gBitsOf<Elements<size>> <= bits)
  constexpr explicit SignedBig(Elements<size> const& elements)
  {
    set(elements);
  }

  template<Size size>
    requires(gBitsOf<Elements<size>> <= bits)
  constexpr auto operator=(Elements<size> const& elements) -> SignedBig&
  {
    set(elements);
    return *this;
  }

  template<Size size>
    requires(signType == Runtime && gBitsOf<Elements<size>> <= bits)
  constexpr explicit SignedBig(Elements<size> const& elements, bool sign)
  {
    set(elements, sign);
  }

  template<Size size>
    requires(!gLimbIsElement && gBitsOf<Limbs<size>> <= bits)
  constexpr explicit SignedBig(Limbs<size> const& elements)
  {
    set(elements);
  }

  template<Size size>
    requires(!gLimbIsElement && gBitsOf<Limbs<size>> <= bits)
  constexpr auto operator=(Limbs<size> const& elements) -> SignedBig&
  {
    set(elements);
    return *this;
  }

  template<Size size>
    requires(signType == Runtime && !gLimbIsElement && gBitsOf<Limbs<size>> <= bits)
  constexpr explicit SignedBig(Limbs<size> const& elements, bool sign)
  {
    set(elements, sign);
  }

  template<SignType otherSignType, Size otherBits>
    requires((signType != Runtime && signType != otherSignType) || otherBits > bits)
  consteval explicit SignedBig(SignedBig<otherSignType, otherBits> const& other)
  {
    set(other.abs().mData, other.sign());
  }

  template<SignType otherSignType, Size otherBits>
    requires((signType != Runtime && signType != otherSignType) || otherBits > bits)
  consteval auto operator=(SignedBig<otherSignType, otherBits> const& other) -> SignedBig&
  {
    set(other.abs().mData, other.sign());
    return *this;
  }

  template<Size otherBits>
    requires(otherBits > bits)
  consteval explicit SignedBig(UnsignedBig<otherBits> const& other)
  {
    set(other.mData);
  }

  template<Size otherBits>
    requires(otherBits > bits)
  consteval auto operator=(UnsignedBig<otherBits> const& other) -> SignedBig&
  {
    set(other.mData);
    return *this;
  }

  template<Size otherBits>
    requires(signType == Runtime && otherBits > bits)
  consteval SignedBig(UnsignedBig<otherBits> const& other, bool sign)
  {
    set(other.mData, sign);
  }

  template<IsUnsignedIntegral Value>
    requires(gBitsOf<Value> > bits)
  consteval explicit SignedBig(Value value)
  {
    set(value);
  }

  template<IsUnsignedIntegral Value>
    requires(gBitsOf<Value> > bits)
  consteval auto operator=(Value value) -> SignedBig&
  {
    set(value);
    return *this;
  }

  template<IsUnsignedIntegral Value>
    requires(signType == Runtime && gBitsOf<Value> > bits)
  consteval explicit SignedBig(Value value, bool sign)
  {
    set(value, sign);
  }

  template<IsSignedIntegral Value>
    requires((signType == Runtime && gBitsOf<Value> > bits) || (signType != Runtime))
  consteval explicit SignedBig(Value value)
  {
    set(value);
  }

  template<IsSignedIntegral Value>
    requires((signType == Runtime && gBitsOf<Value> > bits) || (signType != Runtime))
  consteval auto operator=(Value value) -> SignedBig&
  {
    set(value);
    return *this;
  }

  template<Size size>
    requires(gBitsOf<Elements<size>> > bits)
  consteval explicit SignedBig(Elements<size> const& elements)
  {
    set(elements);
  }

  template<Size size>
    requires(gBitsOf<Elements<size>> > bits)
  consteval auto operator=(Elements<size> const& elements) -> SignedBig&
  {
    set(elements);
    return *this;
  }

  template<Size size>
    requires(signType == Runtime && gBitsOf<Elements<size>> > bits)
  consteval explicit SignedBig(Elements<size> const& elements, bool sign)
  {
    set(elements, sign);
  }

  template<Size size>
    requires(!gLimbIsElement && gBitsOf<Limbs<size>> > bits)
  consteval explicit SignedBig(Limbs<size> const& elements)
  {
    set(elements);
  }

  template<Size size>
    requires(!gLimbIsElement && gBitsOf<Limbs<size>> > bits)
  consteval auto operator=(Limbs<size> const& elements) -> SignedBig&
  {
    set(elements);
    return *this;
  }

  template<Size size>
    requires(signType == Runtime && !gLimbIsElement && gBitsOf<Limbs<size>> > bits)
  consteval explicit SignedBig(Limbs<size> const& elements, bool sign)
  {
    set(elements, sign);
  }

  [[nodiscard]] constexpr auto abs() const -> UnsignedBig<bits> const&
  {
    return std::get<0>(mUnderlying);
  }

  [[nodiscard]] constexpr auto sign() const -> bool
  {
    if constexpr (signType == Runtime) {
      return std::get<1>(mUnderlying);
    }
    return signType == Positive;
  }

private:
  template<typename Value>
    requires(!IsSignedIntegral<Value>)
  constexpr void set(Value const& value)
  {
    set(value, signType != Negative);
  }

  template<IsSignedIntegral Value>
  constexpr void set(Value value)
  {
    using Unsigned       = MakeUnsigned<Value>;
    auto const valueSign = value >= 0;
    auto const valueAbs  = static_cast<Unsigned>(valueSign ? value : -value);
    set(valueAbs, valueSign);
  }

  template<typename Value>
  constexpr void set(Value const& value, bool sign)
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
};

template<SignType otherSignType, Size otherBits>
SignedBig(SignedBig<otherSignType, otherBits> const&) -> SignedBig<otherSignType, otherBits>;

template<Size otherBits>
SignedBig(UnsignedBig<otherBits> const&) -> SignedBig<Positive, otherBits>;

template<Size otherBits>
SignedBig(UnsignedBig<otherBits> const&, bool) -> SignedBig<Runtime, otherBits>;

template<IsUnsignedIntegral Value>
SignedBig(Value) -> SignedBig<Positive, gBitsOf<Value>>;

template<IsUnsignedIntegral Value>
SignedBig(Value, bool) -> SignedBig<Runtime, gBitsOf<Value>>;

template<IsSignedIntegral Value>
SignedBig(Value) -> SignedBig<Runtime, gBitsOf<Value> - 1>;

template<Size size>
SignedBig(Elements<size> const&) -> SignedBig<Positive, gBitsOf<Elements<size>>>;

template<Size size>
SignedBig(Elements<size> const&, bool) -> SignedBig<Runtime, gBitsOf<Elements<size>>>;

template<Size size>
  requires(!gLimbIsElement)
SignedBig(Limbs<size> const&) -> SignedBig<Positive, gBitsOf<Limbs<size>>>;

template<Size size>
  requires(!gLimbIsElement)
SignedBig(Limbs<size> const&, bool) -> SignedBig<Runtime, gBitsOf<Limbs<size>>>;

} // namespace big
