#pragma once

// IWYU pragma: private, include <unsigned.hpp>

#include <unsigned/declaration.hpp>
#include <unsigned/forward.hpp>

#include <common/int.hpp>
#include <common/mask.hpp>
#include <common/type_traits.hpp>

#include <type_traits>

namespace big {

template<Size bits>
template<IsUnsignedBigInit<bits> Value>
  requires(!Is<Value, UnsignedBig<bits>>)
constexpr UnsignedBig<bits>::UnsignedBig(Value&& value)
{
  set<true>(std::forward<Value>(value));
}

template<Size bits>
template<IsUnsignedBigInit<bits> Value>
  requires(!Is<Value, UnsignedBig<bits>>)
constexpr auto
UnsignedBig<bits>::operator=(Value&& value) -> UnsignedBig&
{
  set<true>(std::forward<Value>(value));
  return *this;
}

template<Size bits>
template<IsUnsignedBigConstevalInit<bits> Value>
  requires(!Is<Value, UnsignedBig<bits>>)
consteval UnsignedBig<bits>::UnsignedBig(Value&& value)
{
  set<false>(std::forward<Value>(value));
}

template<Size bits>
template<IsUnsignedBigConstevalInit<bits> Value>
  requires(!Is<Value, UnsignedBig<bits>>)
consteval auto
UnsignedBig<bits>::operator=(Value&& value) -> UnsignedBig&
{
  set<false>(std::forward<Value>(value));
  return *this;
}

// NOLINTBEGIN(*-array-index)

template<Size bits>
constexpr void
UnsignedBig<bits>::element(Size index, Element value)
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

template<Size bits>
constexpr auto
UnsignedBig<bits>::element(Size index) const -> Element
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

template<Size bits>
constexpr auto
UnsignedBig<bits>::max() -> UnsignedBig
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

template<Size bits>
template<bool skipCheck, IsUnsignedBig Value>
constexpr void
UnsignedBig<bits>::set(Value const& value)
{
  set<skipCheck>(value.mData);
}

template<Size bits>
template<bool skipCheck, IsIntegral Value>
constexpr void
UnsignedBig<bits>::set(Value value)
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

template<Size bits>
template<bool skipCheck, Size size>
constexpr void
UnsignedBig<bits>::set(Elements<size> const& elements)
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

template<Size bits>
template<bool skipCheck, Size size>
  requires(!gLimbIsElement)
constexpr void
UnsignedBig<bits>::set(Limbs<size> const& limbs)
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

template<Size bits>
constexpr void
UnsignedBig<bits>::fillZero(Size offset)
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

// NOLINTEND(*-array-index)

} // namespace big
