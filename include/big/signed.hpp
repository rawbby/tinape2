#pragma once

#include <big/common.hpp>   // big::SignType, big::BigElement
#include <big/unsigned.hpp> // big::UnsignedBig
#include <common/tag.hpp>   // Tag, gTag

#include <tuple>       // std::tuple, std::get
#include <type_traits> // std::conditional_t

namespace big {

template<SignType signType, Size bits>
struct SignedBig
{
  [[no_unique_address]] std::conditional_t<signType == Runtime, std::tuple<UnsignedBig<bits>, bool>, std::tuple<UnsignedBig<bits>>> mUnderlying{};

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
    requires(signType != Runtime || gBitsOf<Value> > bits)
  consteval explicit SignedBig(Value value)
  {
    set(value);
  }

  template<IsSignedIntegral Value>
    requires(signType != Runtime || gBitsOf<Value> > bits)
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
  template<typename T>
  constexpr void set(T const& value, bool sign)
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

  template<typename T>
    requires(!IsSignedIntegral<T>)
  constexpr void set(T const& value)
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
};

// ============================================================================
// Constructor Tests
// ============================================================================

static_assert(SignedBig<Positive, 32>{}.sign() == true);
static_assert(SignedBig<Negative, 32>{}.sign() == false);
static_assert(SignedBig<Runtime, 32>{}.sign() == false);

// Unsigned Integral Construction
static_assert(SignedBig<Positive, 32>(42u).abs() == 42);
static_assert(SignedBig<Positive, 32>(42u).sign() == true);

static_assert(SignedBig<Negative, 32>(42u).abs().element(0) == 42);
static_assert(SignedBig<Negative, 32>(42u).sign() == false);

static_assert(SignedBig<Runtime, 32>(42u).abs().element(0) == 42);
static_assert(SignedBig<Runtime, 32>(42u).sign() == true);
static_assert(SignedBig<Runtime, 32>(42u, false).sign() == false);

// Signed Integral Construction
static_assert(SignedBig<Positive, 32>(42).abs().element(0) == 42);
static_assert(SignedBig<Positive, 32>(42).sign() == true);

static_assert(SignedBig<Negative, 32>(-42).abs().element(0) == 42);
static_assert(SignedBig<Negative, 32>(-42).sign() == false);

static_assert(SignedBig<Runtime, 32>(42).abs().element(0) == 42);
static_assert(SignedBig<Runtime, 32>(42).sign() == true);

static_assert(SignedBig<Runtime, 32>(-42).abs().element(0) == 42);
static_assert(SignedBig<Runtime, 32>(-42).sign() == false);

// UnsignedBig Construction
constexpr UnsignedBig<16> gTestUnsigned16{ 100u };
static_assert(SignedBig<Positive, 32>(gTestUnsigned16).abs().element(0) == 100);
static_assert(SignedBig<Runtime, 32>(gTestUnsigned16, false).sign() == false);

// SignedBig Cross-Sign & Cross-Size Construction
constexpr SignedBig<Positive, 16> gTestPos16{ 50 };
static_assert(SignedBig<Positive, 32>(gTestPos16).abs().element(0) == 50);
static_assert(SignedBig<Runtime, 32>(gTestPos16).sign() == true);

constexpr SignedBig<Negative, 64> gTestNeg64{ -50 };
static_assert(SignedBig<Negative, 32>(gTestNeg64).abs().element(0) == 50);
static_assert(SignedBig<Runtime, 32>(gTestNeg64).sign() == false);

constexpr SignedBig<Runtime, 16> gTestRt16Pos{ 50 };
static_assert(SignedBig<Positive, 32>(gTestRt16Pos).abs().element(0) == 50);

constexpr SignedBig<Runtime, 16> gTestRt16Neg{ -50 };
static_assert(SignedBig<Negative, 32>(gTestRt16Neg).abs().element(0) == 50);

// ============================================================================
// Array and Limbs Construction Tests
// ============================================================================

static_assert([]() consteval -> bool {
  constexpr auto elements = Elements<2>{ 1, 2 };
  static_assert(SignedBig<Positive, 64>{ elements }.abs().element(0) == 1);
  static_assert(SignedBig<Positive, 64>{ elements }.abs().element(1) == 2);
  static_assert(SignedBig<Runtime, 64>{ elements, false }.sign() == false);

  constexpr auto limbs = Limbs<2>{ 3, 4 };
  static_assert(SignedBig<Positive, 64>{ limbs }.sign() == true);
  static_assert(SignedBig<Runtime, 64>{ limbs, false }.sign() == false);

  return true;
}());

// ============================================================================
// Assignment Operator Tests
// ============================================================================

consteval auto
testAssignments() -> bool
{
  SignedBig<Runtime, 32> rt{};

  rt = 100u;
  if (rt.abs().element(0) != 100 || rt.sign() != true) return false;

  rt = -100;
  if (rt.abs().element(0) != 100 || rt.sign() != false) return false;

  SignedBig<Positive, 32> pos{};
  pos = gTestPos16;
  if (pos.abs().element(0) != 50 || pos.sign() != true) return false;

  pos = gTestUnsigned16;
  if (pos.abs().element(0) != 100 || pos.sign() != true) return false;

  SignedBig<Negative, 32> neg{};
  neg = gTestNeg64;
  if (neg.abs().element(0) != 50 || neg.sign() != false) return false;

  if constexpr (!gLimbIsElement) {
    Limbs<2> const limbs{ 5, 6 };
    rt = limbs;
    if (rt.sign() != true) return false;
  }

  Elements<2> const elements{ 7, 8 };
  pos = elements;
  if (pos.abs().element(0) != 7 || pos.abs().element(1) != 8) return false;

  return true;
}
static_assert(testAssignments());

} // namespace big
