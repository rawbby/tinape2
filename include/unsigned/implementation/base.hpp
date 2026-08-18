#pragma once

// IWYU pragma: private, include <unsigned.hpp>

#include <unsigned/declaration.hpp>
#include <unsigned/forward.hpp>

#include <common.hpp>

#include <type_traits>

namespace big {
namespace impl {

template<Size bits>
constexpr void
fillZero(UnsignedBig<bits>& self, Size elementOffset)
{
  using Self = UnsignedBig<bits>;

  if constexpr (Self::gIsIntData) {
    if (elementOffset == 0) {
      self.mData = static_cast<Self::Data>(0);
    } else if constexpr (Self::gNumElements > 1) {
      // only the elements starting at the offset must be cleared
      if (elementOffset == 1) {
        self.element(1, static_cast<Element>(0));
      }
    }
  }

  else if constexpr (Self::gIsArrayData) {
    for (Size i = elementOffset; i < Self::gNumElements; ++i) {
      self.mData[i] = static_cast<Element>(0);
    }
  }

  else {
    // safeguard against invalid input
    static_assert(Self::gIsZeroData);
  }
}

template<bool skipCheck, IsIntegral Value, Size bits>
constexpr void
set(UnsignedBig<bits>& self, Value value)
{
  using Self = UnsignedBig<bits>;

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
        if (UnsignedBig<gBitsOf<Unsigned>>{ static_cast<Unsigned>(value) } > Self::max()) {
          throw std::invalid_argument{ "argument does not fit" };
        }
      } else if (UnsignedBig<gBitsOf<Value>>{ value } > Self::max()) {
        throw std::invalid_argument{ "argument does not fit" };
      }
    }
  }

  if constexpr (Self::gIsIntData) {
    self.mData = static_cast<Self::Data>(value);
  }

  else if constexpr (Self::gIsArrayData) {
    if constexpr (IsWide<Value>) {
      self.element(0, loHalf(value));
      self.element(1, hiHalf(value));
      fillZero(self, 2);
    } else {
      self.element(0, static_cast<Element>(value));
      fillZero(self, 1);
    }
  }

  else {
    // safeguard against invalid input
    static_assert(Self::gIsZeroData);
  }
}

template<bool skipCheck, Size size, Size bits>
constexpr void
set(UnsignedBig<bits>& self, Elements<size> const& elements)
{
  using Self = UnsignedBig<bits>;

  if (std::is_constant_evaluated()) {
    if (!skipCheck && UnsignedBig<gBitsOf<Elements<size>>>{ elements } > Self::max()) {
      throw std::invalid_argument{ "argument does not fit" };
    }
  }

  constexpr auto min = std::min(Self::gNumElements, size);
  for (Size i = 0; i < min; ++i) {
    self.element(i, elements[i]);
  }
  if constexpr (Self::gNumElements > size) {
    fillZero(self, size);
  }
}

template<bool skipCheck, Size size, Size bits>
  requires(!gLimbIsElement)
constexpr void
set(UnsignedBig<bits>& self, Limbs<size> const& limbs)
{
  using Self = UnsignedBig<bits>;

  if (std::is_constant_evaluated()) {
    if (!skipCheck && UnsignedBig<gBitsOf<Limbs<size>>>{ limbs } > Self::max()) {
      throw std::invalid_argument{ "argument does not fit" };
    }
  }

  if constexpr (Self::gIsWideData) {
    // we can not write wide data via element if it has not yet been initialized
    self.mData = 0;
  }

  constexpr auto min = std::min(Self::gNumElements * 2, size);
  for (Size i = 0; i < min; i += 2) {
    Element value = limbs[i];
    if (i + 1 < size) {
      value |= static_cast<Element>(limbs[i + 1]) << gBitsOf<Limb>;
    }
    self.element(i / 2, value);
  }
  if constexpr (Self::gNumElements * 2 > size) {
    fillZero(self, (size + 1) / 2);
  }
}

template<bool skipCheck, IsUnsignedBig Value, Size bits>
constexpr void
set(UnsignedBig<bits>& self, Value const& value)
{
  set<skipCheck>(self, value.mData);
}
} // namespace impl

template<Size bits>
template<impl::IsUnsignedBigRhs<bits> Value>
  requires(!Is<Value, UnsignedBig<bits>>)
constexpr UnsignedBig<bits>::UnsignedBig(Value&& value)
{
  impl::set<true>(*this, std::forward<Value>(value));
}

template<Size bits>
template<impl::IsUnsignedBigRhs<bits> Value>
  requires(!Is<Value, UnsignedBig<bits>>)
constexpr auto
UnsignedBig<bits>::operator=(Value&& value) -> UnsignedBig&
{
  impl::set<true>(*this, std::forward<Value>(value));
  return *this;
}

template<Size bits>
template<impl::IsUnsignedBigConstevalRhs<bits> Value>
  requires(!Is<Value, UnsignedBig<bits>>)
consteval UnsignedBig<bits>::UnsignedBig(Value&& value)
{
  impl::set<false>(*this, std::forward<Value>(value));
}

template<Size bits>
template<impl::IsUnsignedBigConstevalRhs<bits> Value>
  requires(!Is<Value, UnsignedBig<bits>>)
consteval auto
UnsignedBig<bits>::operator=(Value&& value) -> UnsignedBig&
{
  impl::set<false>(*this, std::forward<Value>(value));
  return *this;
}

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
constexpr void
UnsignedBig<bits>::limb(Size limbIndex, Limb limb)
  requires(!gIsZeroData)
{
  if constexpr (gLimbIsElement) {
    element(limbIndex, limb);
  } else {
    auto const index = limbIndex >> 1U;
    if (std::is_constant_evaluated()) {
      if (index >= gNumElements) {
        throw std::invalid_argument{ "index out of bounds" };
      }
    }
    auto const shift     = (limbIndex & 1U) == 1U ? gLimbBits : Size{ 0 };
    auto const mask      = static_cast<Element>(gMask<Limb>) << shift;
    auto const asElement = static_cast<Element>(limb) << shift;
    element(index, element(index) & ~mask | asElement);
  }
}

template<Size bits>
constexpr auto
UnsignedBig<bits>::limb(Size limbIndex) const -> Limb
  requires(!gIsZeroData)
{
  if constexpr (gLimbIsElement) {
    return element(limbIndex);
  } else {
    auto const index = limbIndex >> 1U;
    if (std::is_constant_evaluated()) {
      if (index >= gNumElements) {
        throw std::invalid_argument{ "index out of bounds" };
      }
    }
    auto const shift = (limbIndex & 1U) == 1U ? gLimbBits : Size{ 0 };
    return static_cast<Limb>(element(index) >> shift);
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

} // namespace big
