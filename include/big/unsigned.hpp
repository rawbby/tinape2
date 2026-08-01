#pragma once

#include <big/common.hpp> // big::IsBigElement big::IsBigWide, big::loBigElement, big::hiBigElement, big::gBigElementBits, big::gBigWideBits, big::BigElement, big::BigWide

#include <algorithm>   // std::fill, std::min
#include <array>       // std::array
#include <concepts>    // std::integral
#include <stdexcept>   // std::invalid_argument
#include <type_traits> // std::conditional_t, std::is_same_v, std::is_constant_evaluated

namespace big {

template<Size bits>
struct UnsignedBig
{
  constexpr static auto gBits        = bits;
  constexpr static auto gNumElements = (bits + gElementBits - 1) / gElementBits;

  struct ZeroData
  {};

  // clang-format off
  using Data = std::conditional_t<bits == 0, ZeroData,
               std::conditional_t<bits <= gU8Bits, U8,
               std::conditional_t<bits <= gU32Bits, U32,
               std::conditional_t<bits <= gElementBits, Element,
               std::conditional_t<bits <= gWideBits, Wide,
               std::array<Element, gNumElements>>>>>>;
  // clang-format on

  constexpr static auto gIsZeroData  = Is<Data, ZeroData>;
  constexpr static auto gIsArrayData = Is<Data, std::array<Element, gNumElements>>;
  constexpr static auto gIsIntData   = !gIsZeroData && !gIsArrayData;

  constexpr static auto gIsElementData = Is<Data, Element>;
  constexpr static auto gIsWideData    = Is<Data, Wide>;
  constexpr static auto gIsNarrowData  = gIsIntData && !gIsElementData && !gIsWideData;

  // consistency checks and assumptions
  static_assert(!gIsZeroData || gNumElements == 0);
  static_assert(!gIsIntData || gNumElements == 1 || gNumElements == 2);
  static_assert(!gIsArrayData || gNumElements >= 2);
  static_assert(!gIsNarrowData || gNumElements == 1);
  static_assert(!gIsElementData || gNumElements == 1);
  static_assert(!gIsWideData || gNumElements == 2);

  [[no_unique_address]] Data mData;

  constexpr UnsignedBig()                             = default;
  constexpr UnsignedBig(UnsignedBig const& other)     = default;
  constexpr UnsignedBig(UnsignedBig&& other) noexcept = default;
  constexpr ~UnsignedBig()                            = default;

  constexpr auto operator=(UnsignedBig const& other) -> UnsignedBig&     = default;
  constexpr auto operator=(UnsignedBig&& other) noexcept -> UnsignedBig& = default;

  template<Size otherBits>
  constexpr explicit UnsignedBig(
    UnsignedBig<otherBits> const& other)
    requires(otherBits < gBits)
  {
    set<true>(other.mData);
  }

  template<typename Value>
    requires(std::unsigned_integral<Value> || IsElement<Value> || IsWide<Value>)
  constexpr explicit UnsignedBig(
    Value value)
    requires(gBitsOf<Value> <= gBits)
  {
    set<true>(value);
  }

  template<Size size>
  constexpr explicit UnsignedBig(
    std::array<Element,
               size> const& elements)
    requires(size* gElementBits <= gBits)
  {
    set<true>(elements);
  }

  template<Size size>
  constexpr explicit UnsignedBig(
    std::array<U32,
               size> const& elements)
    requires(!Is<Element,
                 U32> &&
             size * gBitsOf<U32> <= gBits)
  {
    set<true>(elements);
  }

  template<Size otherBits>
  constexpr auto operator=(
    UnsignedBig<otherBits> const& other) -> UnsignedBig&
    requires(otherBits <= bits)
  {
    set<true>(other.mData);
    return *this;
  }

  template<typename Value>
    requires(std::unsigned_integral<Value> || IsElement<Value> || IsWide<Value>)
  constexpr auto operator=(
    Value value) -> UnsignedBig&
    requires(gBitsOf<Value> <= gBits)
  {
    set<true>(value);
    return *this;
  }

  template<Size size>
  constexpr auto operator=(
    std::array<Element,
               size> const& elements) -> UnsignedBig&
    requires(size* gElementBits <= gBits)
  {
    set<true>(elements);
    return *this;
  }

  template<Size size>
  constexpr auto operator=(
    std::array<U32,
               size> const& elements) -> UnsignedBig&
    requires(!Is<Element,
                 U32> &&
             size * gBitsOf<U32> <= gBits)
  {
    set<true>(elements);
    return *this;
  }

  template<Size otherBits>
  consteval explicit UnsignedBig(
    UnsignedBig<otherBits> const& other)
    requires(otherBits > gBits)
  {
    set<false>(other.mData);
  }

  template<typename Value>
    requires(std::unsigned_integral<Value> || IsElement<Value> || IsWide<Value>)
  consteval explicit UnsignedBig(
    Value value)
    requires(gBitsOf<Value> > gBits)
  {
    set<false>(value);
  }

  template<typename Value>
    requires(std::integral<Value> && !std::unsigned_integral<Value>)
  consteval explicit UnsignedBig(
    Value value)
  {
    set<false>(value);
  }

  template<Size size>
  consteval explicit UnsignedBig(
    std::array<Element,
               size> const& elements)
    requires(size* gElementBits > gBits)
  {
    set<false>(elements);
  }

  template<Size size>
  consteval explicit UnsignedBig(
    std::array<U32,
               size> const& elements)
    requires(!Is<Element,
                 U32> &&
             size * gBitsOf<U32> > gBits)
  {
    set<false>(elements);
  }

  template<Size otherBits>
  consteval auto operator=(
    UnsignedBig<otherBits> const& other) -> UnsignedBig&
    requires(otherBits > bits)
  {
    set<false>(other.mData);
    return *this;
  }

  template<typename Value>
    requires(std::unsigned_integral<Value> || IsElement<Value> || IsWide<Value>)
  consteval auto operator=(
    Value value) -> UnsignedBig&
    requires(gBitsOf<Value> > gBits)
  {
    set<false>(value);
    return *this;
  }

  template<typename Value>
    requires(std::integral<Value> && !std::unsigned_integral<Value>)
  consteval auto operator=(
    Value value) -> UnsignedBig&
  {
    set<false>(value);
    return *this;
  }

  template<Size size>
  consteval auto operator=(
    std::array<Element,
               size> const& elements) -> UnsignedBig&
    requires(size* gElementBits > gBits)
  {
    set<false>(elements);
    return *this;
  }

  template<Size size>
  consteval auto operator=(
    std::array<U32,
               size> const& elements) -> UnsignedBig&
    requires(!Is<Element,
                 U32> &&
             size * gBitsOf<U32> > gBits)
  {
    set<false>(elements);
    return *this;
  }

  // NOLINTBEGIN(*-constant-array-index)
  constexpr void element(
    Size    index,
    Element value)
  {
    if (std::is_constant_evaluated()) {
      if (index >= gNumElements) {
        throw std::invalid_argument{ "index out of bounds" };
      }
    }

    if constexpr (gIsZeroData) {
      // todo zero data should not be setted
    }

    else if constexpr (gIsArrayData) {
      mData[index] = value;
    }

    else if constexpr (gNumElements == 1) {
      mData = static_cast<Data>(value);
    }

    else {
      // safeguard against invalid input
      static_assert(gNumElements == 2);

      constexpr auto mask = (static_cast<Data>(1) << gElementBits) - 1;
      if (index == 0) {
        mData = (mData & (mask << gElementBits)) | value;
      } else {
        mData = (mData & mask) | (static_cast<Data>(value) << gElementBits);
      }
    }
  }

  [[nodiscard]] constexpr auto element(
    Size index) const -> Element
  {
    if (std::is_constant_evaluated()) {
      if (index >= gNumElements) {
        throw std::invalid_argument{ "index out of bounds" };
      }
    }

    if constexpr (gIsZeroData) {
      // todo zero data should not be getted
      return static_cast<Element>(0);
    }

    else if constexpr (gIsArrayData) {
      return mData[index];
    }

    else if constexpr (gNumElements == 1) {
      return static_cast<Element>(mData);
    }

    else {
      // safeguard against invalid input
      static_assert(gNumElements == 2);

      if (index == 0) {
        return static_cast<Element>(mData);
      }
      return static_cast<Element>(mData >> gElementBits);
    }
  }

  constexpr static auto max() -> UnsignedBig
  {
    UnsignedBig result{};
    if constexpr (gIsZeroData) {
      return result;
    }

    else if constexpr (gIsIntData) {
      constexpr auto shift = gBitsOf<Data> - gBits;

      result.mData = static_cast<Data>(static_cast<Data>(-1) >> shift);
      return result;
    }

    else {
      // safeguard against invalid input
      static_assert(gIsArrayData);
      constexpr auto remainder = gBits % gElementBits;
      constexpr auto backBits  = remainder == 0 ? gElementBits : remainder;
      for (Size i = 0; i < gNumElements - 1; ++i) {
        result.mData[i] = gMask<Element>;
      }
      result.mData[gNumElements - 1] = gLoMask<Element, backBits>;
      return result;
    }
  }

private:
  template<bool skipCheck,
           typename Value>
    requires(std::integral<Value> || IsElement<Value> || IsWide<Value>)
  constexpr void set(
    Value value)
  {
    if (std::is_constant_evaluated()) {
      if (!skipCheck) {
        if (value < 0) {
          throw std::invalid_argument{ "argument must be non-negative" };
        }
        if constexpr (std::signed_integral<Value>) {
          using UValue = std::make_unsigned_t<Value>;
          if (UnsignedBig<gBitsOf<UValue>>{ static_cast<UValue>(value) } > max()) {
            throw std::invalid_argument{ "argument does not fit" };
          }
        } else if (UnsignedBig<gBitsOf<Value>>{ value } > max()) {
          throw std::invalid_argument{ "argument does not fit" };
        }
      }
    }

    if constexpr (gIsIntData) {
      mData = static_cast<Data>(value);
    }

    else if constexpr (gIsArrayData) {
      if constexpr (IsWide<Value>) {
        element(0, loElement(value));
        element(1, hiElement(value));
        fillZero(2);
      } else {
        element(0, static_cast<Element>(value));
        fillZero(1);
      }
    }

    else {
      // safeguard against invalid input
      static_assert(gIsZeroData);
    }
  }

  template<bool skipCheck,
           Size size>
  constexpr void set(
    std::array<Element,
               size> const& elements)
  {
    if (std::is_constant_evaluated()) {
      if (!skipCheck && UnsignedBig<size * gElementBits>{ elements } > max()) {
        throw std::invalid_argument{ "argument does not fit" };
      }
    }

    constexpr auto min = std::min(gNumElements, size);
    for (Size i = 0; i < min; ++i) {
      element(i, elements[i]);
    }
    if constexpr (gNumElements > size) {
      fillZero(size);
    }
  }

  template<bool skipCheck,
           Size size>
    requires(!Is<Element,
                 U32>)
  constexpr void set(
    std::array<U32,
               size> const& elements)
  {
    static_assert(gElementBytes == gU32Bytes * 2);

    if (std::is_constant_evaluated()) {
      if (!skipCheck && UnsignedBig<size * gBitsOf<U32>>{ elements } > max()) {
        throw std::invalid_argument{ "argument does not fit" };
      }
    }

    if constexpr (gIsWideData) {
      // we can not write wide data via element if it has not yet been initialized
      mData = 0;
    }

    constexpr auto min = std::min(gNumElements * 2, size);
    for (Size i = 0; i < min; i += 2) {
      Element value = elements[i];
      if (i + 1 < size) {
        value |= static_cast<Element>(elements[i + 1]) << gBitsOf<U32>;
      }
      element(i / 2, value);
    }
    if constexpr (gNumElements * 2 > size) {
      fillZero((size + 1) / 2);
    }
  }

  constexpr void fillZero(
    Size offset = 0)
  {
    if constexpr (gIsIntData) {
      if (offset == 0) {
        mData = static_cast<Data>(0);
      } else if constexpr (gNumElements > 1) {
        element(1, static_cast<Element>(0));
      }
    }

    else if constexpr (gIsArrayData) {
      for (Size i = offset; i < gNumElements; ++i) {
        mData[i] = static_cast<Element>(0);
      }
    }

    else {
      // safeguard against invalid input
      static_assert(gIsZeroData);
    }
  }
  // NOLINTEND(*-constant-array-index)
};

UnsignedBig() -> UnsignedBig<0>;

template<typename Value>
  requires(std::unsigned_integral<Value> || IsElement<Value> || IsWide<Value>)
UnsignedBig(Value) -> UnsignedBig<gBitsOf<Value>>;

template<typename Value>
  requires(std::signed_integral<Value>)
UnsignedBig(Value) -> UnsignedBig<gBitsOf<Value> - 1>;

template<Size size>
UnsignedBig(std::array<Element,
                       size>) -> UnsignedBig<size * gElementBits>;

template<Size size>
  requires(!Is<Element,
               U32>)
UnsignedBig(std::array<U32,
                       size>) -> UnsignedBig<size * gBitsOf<U32>>;

} // namespace big
