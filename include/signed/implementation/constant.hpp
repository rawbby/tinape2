#pragma once

// IWYU pragma: private, include <signed.hpp>

#include <signed/declaration.hpp>
#include <signed/forward.hpp>

#include <common.hpp>
#include <unsigned.hpp>

#include <array>

namespace big {

template<auto value>
  requires requires { SignedBig{ gTag<Runtime>, value }; }
constexpr auto gSignedBig = [] {
  constexpr auto base = SignedBig{ gTag<Runtime>, value };
  using Base          = decltype(base);

  constexpr auto neededUintBits = [](auto uint) {
    if (uint == 0) {
      return Size{ 0 };
    }

    Size msi = 1;
    for (Size shift = 0; uint > 1 && shift < Base::gBits; ++shift) {
      uint >>= 1U;
      ++msi;
    }
    return msi;
  };

  constexpr auto neededArrayBits = [neededUintBits]<Size size>(std::array<Element, size> const & elements) {
    for (Size i = elements.size(); i > 0;) {
      --i;
      if (elements[i] != Element{ 0 }) {
        return i * gElementBits + neededUintBits(elements[i]);
      }
    }
    return Size{ 0 };
  };

  if constexpr (Base::gIsIntData && value < 0) {
    return SignedBig<SignType::Negative, neededUintBits(base.abs().mData)>{ base.abs().mData };

  } else if constexpr (Base::gIsIntData && value >= 0) {
    return SignedBig<SignType::Positive, neededUintBits(base.abs().mData)>{ base.abs().mData };

  } else if constexpr (Base::gIsArrayData && value < 0) {
    return SignedBig<SignType::Negative, neededArrayBits(base.abs().mData)>{ base.abs().mData };

  } else if constexpr (Base::gIsArrayData && value >= 0) {
    return SignedBig<SignType::Positive, neededArrayBits(base.abs().mData)>{ base.abs().mData };

  } else {
    static_assert(Base::gIsZeroData);
    return SignedBig{};
  }
}();

} // namespace big
