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
               Elements<gNumElements>>>>>>;
  // clang-format on

  constexpr static auto gIsZeroData  = Is<Data, ZeroData>;
  constexpr static auto gIsArrayData = Is<Data, Elements<gNumElements>>;
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
  constexpr ~UnsignedBig()                            = default;
  constexpr UnsignedBig(UnsignedBig const& other)     = default;
  constexpr UnsignedBig(UnsignedBig&& other) noexcept = default;

  constexpr auto operator=(UnsignedBig const& other) -> UnsignedBig&     = default;
  constexpr auto operator=(UnsignedBig&& other) noexcept -> UnsignedBig& = default;

  template<Size otherBits>
    requires(otherBits < gBits)
  constexpr explicit UnsignedBig(UnsignedBig<otherBits> const& other)
  {
    set<true>(other.mData);
  }

  template<IsUnsignedIntegral Value>
    requires(gBitsOf<Value> <= gBits)
  constexpr explicit UnsignedBig(Value value)
  {
    set<true>(value);
  }

  template<Size size>
    requires(gBitsOf<Elements<size>> <= gBits)
  constexpr explicit UnsignedBig(Elements<size> const& elements)
  {
    set<true>(elements);
  }

  template<Size size>
    requires(!gLimbIsElement && gBitsOf<Limbs<size>> <= gBits)
  constexpr explicit UnsignedBig(Limbs<size> const& limbs)
  {
    set<true>(limbs);
  }

  template<Size otherBits>
    requires(otherBits <= bits)
  constexpr auto operator=(UnsignedBig<otherBits> const& other) -> UnsignedBig&
  {
    set<true>(other.mData);
    return *this;
  }

  template<IsUnsignedIntegral Value>
    requires(gBitsOf<Value> <= gBits)
  constexpr auto operator=(Value value) -> UnsignedBig&
  {
    set<true>(value);
    return *this;
  }

  template<Size size>
    requires(gBitsOf<Elements<size>> <= gBits)
  constexpr auto operator=(Elements<size> const& elements) -> UnsignedBig&
  {
    set<true>(elements);
    return *this;
  }

  template<Size size>
    requires(!gLimbIsElement && gBitsOf<Limbs<size>> <= gBits)
  constexpr auto operator=(Limbs<size> const& elements) -> UnsignedBig&
  {
    set<true>(elements);
    return *this;
  }

  template<Size otherBits>
    requires(otherBits > gBits)
  consteval explicit UnsignedBig(UnsignedBig<otherBits> const& other)
  {
    set<false>(other.mData);
  }

  template<IsUnsignedIntegral Value>
    requires(gBitsOf<Value> > gBits)
  consteval explicit UnsignedBig(Value value)
  {
    set<false>(value);
  }

  template<IsSignedIntegral Value>
  consteval explicit UnsignedBig(Value value)
  {
    set<false>(value);
  }

  template<Size size>
    requires(gBitsOf<Elements<size>> > gBits)
  consteval explicit UnsignedBig(Elements<size> const& elements)
  {
    set<false>(elements);
  }

  template<Size size>
    requires(!gLimbIsElement && gBitsOf<Limbs<size>> > gBits)
  consteval explicit UnsignedBig(Limbs<size> const& elements)
  {
    set<false>(elements);
  }

  template<Size otherBits>
    requires(otherBits > bits)
  consteval auto operator=(UnsignedBig<otherBits> const& other) -> UnsignedBig&
  {
    set<false>(other.mData);
    return *this;
  }

  template<IsUnsignedIntegral Value>
    requires(gBitsOf<Value> > gBits)
  consteval auto operator=(Value value) -> UnsignedBig&
  {
    set<false>(value);
    return *this;
  }

  template<IsSignedIntegral Value>
  consteval auto operator=(Value value) -> UnsignedBig&
  {
    set<false>(value);
    return *this;
  }

  template<Size size>
    requires(gBitsOf<Elements<size>> > gBits)
  consteval auto operator=(Elements<size> const& elements) -> UnsignedBig&
  {
    set<false>(elements);
    return *this;
  }

  template<Size size>
    requires(!gLimbIsElement && gBitsOf<Limbs<size>> > gBits)
  consteval auto operator=(Limbs<size> const& elements) -> UnsignedBig&
  {
    set<false>(elements);
    return *this;
  }

  // NOLINTBEGIN(*-constant-array-index)
  constexpr void element(Size index, Element value)
    requires(!gIsZeroData)
  {
    if (std::is_constant_evaluated()) {
      if (index >= gNumElements) {
        throw std::invalid_argument{ "index out of bounds" };
      }
    }

    if constexpr (gIsArrayData) {
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

  [[nodiscard]] constexpr auto element(Size index) const -> Element
    requires(!gIsZeroData)
  {
    if (std::is_constant_evaluated()) {
      if (index >= gNumElements) {
        throw std::invalid_argument{ "index out of bounds" };
      }
    }

    if constexpr (gIsArrayData) {
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
  template<bool skipCheck, IsIntegral Value>
  constexpr void set(Value value)
  {
    if (std::is_constant_evaluated()) {
      if (!skipCheck) {
        if (value < 0) {
          throw std::invalid_argument{ "argument must be non-negative" };
        }
        if constexpr (IsSignedIntegral<Value>) {
          if (value < 0) {
            throw std::invalid_argument{ "argument does not fit" };
          }
          using Unsigned = MakeUnsigned<Value>;
          if (UnsignedBig<gBitsOf<Unsigned>>{ static_cast<Unsigned>(value) } > max()) {
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
        element(0, loHalf(value));
        element(1, hiHalf(value));
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

  template<bool skipCheck, Size size>
  constexpr void set(Elements<size> const& elements)
  {
    if (std::is_constant_evaluated()) {
      if (!skipCheck && UnsignedBig<gBitsOf<Elements<size>>>{ elements } > max()) {
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

  template<bool skipCheck, Size size>
    requires(!gLimbIsElement)
  constexpr void set(Limbs<size> const& limbs)
  {
    static_assert(gElementBytes == gLimbBytes * 2);

    if (std::is_constant_evaluated()) {
      if (!skipCheck && UnsignedBig<gBitsOf<Limbs<size>>>{ limbs } > max()) {
        throw std::invalid_argument{ "argument does not fit" };
      }
    }

    if constexpr (gIsWideData) {
      // we can not write wide data via element if it has not yet been initialized
      mData = 0;
    }

    constexpr auto min = std::min(gNumElements * 2, size);
    for (Size i = 0; i < min; i += 2) {
      Element value = limbs[i];
      if (i + 1 < size) {
        value |= static_cast<Element>(limbs[i + 1]) << gBitsOf<Limb>;
      }
      element(i / 2, value);
    }
    if constexpr (gNumElements * 2 > size) {
      fillZero((size + 1) / 2);
    }
  }

  constexpr void fillZero(Size offset = 0)
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

template<IsUnsignedIntegral Value>
UnsignedBig(Value) -> UnsignedBig<gBitsOf<Value>>;

template<IsSignedIntegral Value>
UnsignedBig(Value) -> UnsignedBig<gBitsOf<Value> - 1>;

template<Size size>
UnsignedBig(Elements<size>) -> UnsignedBig<gBitsOf<Elements<size>>>;

template<Size size>
  requires(!gLimbIsElement)
UnsignedBig(Limbs<size>) -> UnsignedBig<gBitsOf<Limbs<size>>>;

} // namespace big
