#pragma once

// IWYU pragma: private, include <unsigned.hpp>

#include <unsigned/forward.hpp>

#include <common/int.hpp>
#include <common/type_traits.hpp>

#include <compare>
#include <optional>
#include <type_traits>

namespace big {

template<Size bits>
class UnsignedBig final
{
  using Traits = impl::IsUnsignedBigHelper<UnsignedBig<bits>>;

public:
  using Data = Traits::Data;

  constexpr static auto gBits          = Traits::gBits;
  constexpr static auto gNumElements   = Traits::gNumElements;
  constexpr static auto gIsZeroData    = Traits::gIsZeroData;
  constexpr static auto gIsArrayData   = Traits::gIsArrayData;
  constexpr static auto gIsIntData     = Traits::gIsIntData;
  constexpr static auto gIsElementData = Traits::gIsElementData;
  constexpr static auto gIsWideData    = Traits::gIsWideData;
  constexpr static auto gIsNarrowData  = Traits::gIsNarrowData;

  [[no_unique_address]] Data mData;

  constexpr UnsignedBig()                             = default;
  constexpr ~UnsignedBig()                            = default;
  constexpr UnsignedBig(UnsignedBig const& other)     = default;
  constexpr UnsignedBig(UnsignedBig&& other) noexcept = default;

  constexpr auto operator=(UnsignedBig const& other) -> UnsignedBig&     = default;
  constexpr auto operator=(UnsignedBig&& other) noexcept -> UnsignedBig& = default;

  template<impl::IsUnsignedBigInit<bits> Value>
    requires(!Is<Value, UnsignedBig<bits>>)
  constexpr explicit UnsignedBig(Value&& value);

  template<impl::IsUnsignedBigInit<bits> Value>
    requires(!Is<Value, UnsignedBig<bits>>)
  constexpr auto operator=(Value&& value) -> UnsignedBig&;

  template<impl::IsUnsignedBigConstevalInit<bits> Value>
    requires(!Is<Value, UnsignedBig<bits>>)
  consteval explicit UnsignedBig(Value&& value);

  template<impl::IsUnsignedBigConstevalInit<bits> Value>
    requires(!Is<Value, UnsignedBig<bits>>)
  consteval auto operator=(Value&& value) -> UnsignedBig&;

  constexpr void element(Size index, Element value)
    requires(!gIsZeroData);

  [[nodiscard]] constexpr auto element(Size index) const -> Element
    requires(!gIsZeroData);

  constexpr static auto max() -> UnsignedBig;

private:
  template<bool skipCheck, IsUnsignedBig Value>
  constexpr void set(Value const& value);

  template<bool skipCheck, IsIntegral Value>
  constexpr void set(Value value);

  template<bool skipCheck, Size size>
  constexpr void set(Elements<size> const& elements);

  template<bool skipCheck, Size size>
    requires(!gLimbIsElement)
  constexpr void set(Limbs<size> const& limbs);

  constexpr void fillZero(Size offset = 0);
};

UnsignedBig() -> UnsignedBig<0>;

template<Size bits>
UnsignedBig(UnsignedBig<bits>) -> UnsignedBig<bits>;

template<typename Value>
  requires(IsUnsignedIntegral<Value> || IsElements<Value> || IsLimbs<Value>)
UnsignedBig(Value) -> UnsignedBig<gBitsOf<Value>>;

template<IsSignedIntegral Value>
UnsignedBig(Value) -> UnsignedBig<gBitsOf<Value> - 1>;

template<Size lhsBits, Size rhsBits>
constexpr auto
operator+(UnsignedBig<lhsBits> const& lhs, UnsignedBig<rhsBits> const& rhs);

template<Size lhsBits, Size rhsBits>
constexpr auto
operator-(UnsignedBig<lhsBits> const& lhs, UnsignedBig<rhsBits> const& rhs);

template<Size lhsBits, Size rhsBits>
constexpr auto
operator*(UnsignedBig<lhsBits> const& lhs, UnsignedBig<rhsBits> const& rhs);

template<typename Rhs>
  requires(!IsUnsignedBig<Rhs>) && requires(Rhs const& rhs) { UnsignedBig{ rhs }; }
constexpr auto
operator+(IsUnsignedBig auto const& lhs, Rhs const& rhs);

template<typename Lhs>
  requires(!IsUnsignedBig<Lhs>) && requires(Lhs const& lhs) { UnsignedBig{ lhs }; }
constexpr auto
operator+(Lhs const& lhs, IsUnsignedBig auto const& rhs);

template<typename Rhs>
  requires(!IsUnsignedBig<Rhs>) && requires(Rhs const& rhs) { UnsignedBig{ rhs }; }
constexpr auto
operator-(IsUnsignedBig auto const& lhs, Rhs const& rhs);

template<typename Lhs>
  requires(!IsUnsignedBig<Lhs>) && requires(Lhs const& lhs) { UnsignedBig{ lhs }; }
constexpr auto
operator-(Lhs const& lhs, IsUnsignedBig auto const& rhs);

template<typename Rhs>
  requires(!IsUnsignedBig<Rhs>) && requires(Rhs const& rhs) { UnsignedBig{ rhs }; }
constexpr auto
operator*(IsUnsignedBig auto const& lhs, Rhs const& rhs);

template<typename Lhs>
  requires(!IsUnsignedBig<Lhs>) && requires(Lhs const& lhs) { UnsignedBig{ lhs }; }
constexpr auto
operator*(Lhs const& lhs, IsUnsignedBig auto const& rhs);

template<IsUnsignedBig Lhs, IsUnsignedBig Rhs>
constexpr auto
operator<=>(Lhs const& lhs, Rhs const& rhs) -> std::strong_ordering;

template<IsUnsignedBig Lhs, IsUnsignedBig Rhs>
constexpr auto
operator==(Lhs const& lhs, Rhs const& rhs) -> bool;

template<typename Rhs>
  requires(!IsUnsignedBig<Rhs>) && requires(Rhs const& rhs) { UnsignedBig{ rhs }; }
constexpr auto
operator<=>(IsUnsignedBig auto const& lhs, Rhs const& rhs);

template<typename Lhs>
  requires(!IsUnsignedBig<Lhs>) && requires(Lhs const& lhs) { UnsignedBig{ lhs }; }
constexpr auto
operator<=>(Lhs const& lhs, IsUnsignedBig auto const& rhs);

template<typename Rhs>
  requires(!IsUnsignedBig<Rhs>) && requires(Rhs const& rhs) { UnsignedBig{ rhs }; }
constexpr auto
operator==(IsUnsignedBig auto const& lhs, Rhs const& rhs);

template<typename Lhs>
  requires(!IsUnsignedBig<Lhs>) && requires(Lhs const& lhs) { UnsignedBig{ lhs }; }
constexpr auto
operator==(Lhs const& lhs, IsUnsignedBig auto const& rhs);

/// truncation based cast without extensive runtime checking but simple (exact) masking
template<IsUnsignedBig ToUnsignedBig, IsUnsignedBig FromUnsignedBig>
[[nodiscard]] constexpr auto
castBigUnsafe(FromUnsignedBig const& value) -> ToUnsignedBig;

/// safe casting returns a valid value if fromValue fits toValue or std::nullopt
template<IsUnsignedBig ToUnsignedBig, IsUnsignedBig FromUnsignedBig>
[[nodiscard]] constexpr auto
castBigSafe(FromUnsignedBig const& value) -> std::optional<ToUnsignedBig>;

} // namespace big
