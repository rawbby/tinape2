#pragma once

// IWYU pragma: private, include <unsigned.hpp>

#include <unsigned/declaration.hpp>
#include <unsigned/forward.hpp>

#include <common.hpp>

namespace big {

template<Size bits>
template<Size maskBits>
constexpr auto
UnsignedBig<bits>::hiMask() -> UnsignedBig
{
  auto result = UnsignedBig<bits>{};
  if constexpr (gIsIntData) {
    result.mData = gLoMask<Data, maskBits>;
    // prevent undefined behaviour
    if constexpr (gBitsOf<Data> > (bits - maskBits)) {
      result.mData <<= bits - maskBits;
    }
  }
  if constexpr (gIsArrayData) {
    constexpr auto msbBits = bits % gElementBits;
    if constexpr (msbBits > maskBits) {
      result.mData[(bits - 1) / gElementBits] = gLoMask<Element, maskBits> << (msbBits - maskBits);
    } else {
      constexpr auto lsbBits    = (maskBits - msbBits) % gElementBits;
      constexpr auto beginIndex = (bits - maskBits) / gElementBits;
      constexpr auto backIndex  = (bits - 1) / gElementBits;

      result.mData[beginIndex] = lsbBits == 0 ? gMask<Element> : gHiMask<Element, lsbBits>;
      for (Size i = beginIndex + 1; i < backIndex; ++i) {
        result.mData[i] = gMask<Element>; // NOLINT(*-constant-array-index)
      }
      result.mData[backIndex] = msbBits == 0 ? gMask<Element> : gLoMask<Element, msbBits>;
    }
  }
  return result;
}

template<Size bits>
template<Size maskBits>
constexpr auto
UnsignedBig<bits>::loMask() -> UnsignedBig
{
  auto result = UnsignedBig<bits>{};
  if constexpr (gIsIntData) {
    result.mData = gLoMask<Data, maskBits>;
  }
  if constexpr (gIsArrayData) {
    constexpr auto end  = maskBits / gElementBits;
    constexpr auto mask = maskBits % gElementBits;
    for (Size i = 0; i < end; ++i) {
      result.mData[i] = gMask<Element>; // NOLINT(*-constant-array-index)
    }
    if constexpr (mask != 0) {
      result.mData[end] = gLoMask<Element, mask>;
    }
  }
  return result;
}

template<Size bits>
constexpr auto
UnsignedBig<bits>::operator&=(UnsignedBig const& rhs) -> UnsignedBig&
{
  if constexpr (UnsignedBig<bits>::gIsIntData) {
    mData &= rhs.mData;
  }
  if constexpr (UnsignedBig<bits>::gIsArrayData) {
    for (Size i = 0; i < mData.size(); ++i) {
      mData[i] &= rhs.mData[i];
    }
  }
  return *this;
}

template<Size bits>
constexpr auto
UnsignedBig<bits>::operator|=(UnsignedBig const& rhs) -> UnsignedBig&
{
  if constexpr (UnsignedBig<bits>::gIsIntData) {
    mData |= rhs.mData;
  }
  if constexpr (UnsignedBig<bits>::gIsArrayData) {
    for (Size i = 0; i < mData.size(); ++i) {
      mData[i] |= rhs.mData[i];
    }
  }
  return *this;
}

template<Size bits>
constexpr auto
UnsignedBig<bits>::operator^=(UnsignedBig const& rhs) -> UnsignedBig&
{
  if constexpr (UnsignedBig<bits>::gIsIntData) {
    mData ^= rhs.mData;
  }
  if constexpr (UnsignedBig<bits>::gIsArrayData) {
    for (Size i = 0; i < mData.size(); ++i) {
      mData[i] ^= rhs.mData[i];
    }
  }
  return *this;
}

template<Size bits>
constexpr auto
operator&(UnsignedBig<bits> const& lhs, UnsignedBig<bits> const& rhs) -> UnsignedBig<bits>
{
  auto result = UnsignedBig<bits>{};

  if constexpr (UnsignedBig<bits>::gIsIntData) {
    result.mData = lhs.mData & rhs.mData;
  }
  if constexpr (UnsignedBig<bits>::gIsArrayData) {
    for (Size i = 0; i < result.mData.size(); ++i) {
      result.mData[i] = lhs.mData[i] & rhs.mData[i];
    }
  }

  return result;
}

template<Size bits>
constexpr auto
operator|(UnsignedBig<bits> const& lhs, UnsignedBig<bits> const& rhs) -> UnsignedBig<bits>
{
  auto result = UnsignedBig<bits>{};

  if constexpr (UnsignedBig<bits>::gIsIntData) {
    result.mData = lhs.mData | rhs.mData;
  }
  if constexpr (UnsignedBig<bits>::gIsArrayData) {
    for (Size i = 0; i < result.mData.size(); ++i) {
      result.mData[i] = lhs.mData[i] | rhs.mData[i];
    }
  }

  return result;
}

template<Size bits>
constexpr auto
operator^(UnsignedBig<bits> const& lhs, UnsignedBig<bits> const& rhs) -> UnsignedBig<bits>
{
  auto result = UnsignedBig<bits>{};

  if constexpr (UnsignedBig<bits>::gIsIntData) {
    result.mData = lhs.mData ^ rhs.mData;
  }
  if constexpr (UnsignedBig<bits>::gIsArrayData) {
    for (Size i = 0; i < result.mData.size(); ++i) {
      result.mData[i] = lhs.mData[i] ^ rhs.mData[i];
    }
  }

  return result;
}

template<Size bits>
constexpr auto
operator~(UnsignedBig<bits> const& value) -> UnsignedBig<bits>
{
  auto result = UnsignedBig<bits>{};

  if constexpr (UnsignedBig<bits>::gIsIntData) {
    result.mData = ~value.mData;
  }
  if constexpr (UnsignedBig<bits>::gIsArrayData) {
    constexpr auto endIndex  = result.mData.size();
    constexpr auto backIndex = endIndex - 1;
    for (Size i = 0; i < backIndex; ++i) {
      // NOLINTNEXTLINE(*-constant-array-index)
      result.mData[i] = ~value.mData[i];
    }
    constexpr auto remainder = bits - (backIndex * gElementBits);
    constexpr auto mask      = gHiMask<Element, gElementBits - remainder>;
    result.mData[backIndex]  = ~value.mData[backIndex] ^ mask;
  }

  return result;
}

} // namespace big
