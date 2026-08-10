#pragma once

// IWYU pragma: private, include <unsigned.hpp>

#include <unsigned/declaration.hpp>
#include <unsigned/forward.hpp>

#include <common/int.hpp>

#include <array>

namespace big {

template<auto value>
  requires requires { UnsignedBig{ value }; }
constexpr auto gUnsignedBig = [] {
  constexpr auto base = UnsignedBig{ value };
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
      if (elements[i] != Element{ 0 }) {                         // NOLINT(*-constant-array-index)
        return (i * gElementBits) + neededUintBits(elements[i]); // NOLINT(*-constant-array-index)
      }
    }
    return Size{ 0 };
  };

  if constexpr (Base::gIsZeroData) {
    return UnsignedBig{};

  } else if constexpr (Base::gIsIntData) {
    return UnsignedBig<neededUintBits(base.mData)>{ base.mData };

  } else {
    static_assert(Base::gIsArrayData);
    return UnsignedBig<neededArrayBits(base.mData)>{ value };
  }
}();

} // namespace big
