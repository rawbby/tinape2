#pragma once

// IWYU pragma: private, include <unsigned.hpp>

#include <unsigned/declaration.hpp>
#include <unsigned/forward.hpp>
#include <unsigned/implementation/karatsuba.hpp>

#include <common/int.hpp>
#include <common/mask.hpp>

#include <algorithm>
#include <stdexcept>
#include <type_traits>

namespace big {

template<Size lhsBits, Size rhsBits>
constexpr auto
operator+(UnsignedBig<lhsBits> const& lhs, UnsignedBig<rhsBits> const& rhs)
{
  if constexpr (UnsignedBig<rhsBits>::gIsZeroData) {
    return lhs;
  }

  else if constexpr (UnsignedBig<lhsBits>::gIsZeroData) {
    return rhs;
  }

  else if constexpr (rhsBits > lhsBits) {
    return rhs + lhs;

  } else {
    constexpr auto resBits = rhsBits > 0 ? lhsBits + 1 : lhsBits;
    using ResType          = UnsignedBig<resBits>;
    using LhsType          = UnsignedBig<lhsBits>;
    using RhsType          = UnsignedBig<rhsBits>;

    auto sum = ResType{};

    if constexpr (!ResType::gIsArrayData) {
      sum.mData = static_cast<ResType::Data>(static_cast<ResType::Data>(lhs.mData) + rhs.mData);

    } else {
      // NOLINTBEGIN(*-magic-numbers,*-identifier-length,*-unchecked-container-access)

      auto carry = Element{ 0 };
      auto i     = Size{ 0 };

      // loop common elements for all
      for (; i < RhsType::gNumElements; ++i) {
        auto const elementSum = static_cast<Wide>(lhs.element(i)) + rhs.element(i) + carry;
        sum.element(i, static_cast<Element>(elementSum));
        carry = static_cast<Element>(elementSum >> gElementBits);
      }

      // add lhs for diff
      for (; i < LhsType::gNumElements; ++i) {
        auto const elementSum = static_cast<Wide>(lhs.element(i)) + carry;
        sum.element(i, static_cast<Element>(elementSum));
        carry = static_cast<Element>(elementSum >> gElementBits);
      }

      // carry for result having an element more than lhs
      if constexpr (ResType::gNumElements > LhsType::gNumElements) {
        sum.element(i, carry);
      }
      // NOLINTEND(*-magic-numbers,*-identifier-length,*-unchecked-container-access)
    }

    return sum;
  }
}

template<Size lhsBits, Size rhsBits>
constexpr auto
operator-(UnsignedBig<lhsBits> const& lhs, UnsignedBig<rhsBits> const& rhs)
{
  if (std::is_constant_evaluated() && rhs > lhs) {
    throw std::invalid_argument{ "undefined behaviour" };
  }

  if constexpr (UnsignedBig<rhsBits>::gIsZeroData) {
    return lhs;
  }

  else if constexpr (UnsignedBig<lhsBits>::gIsZeroData) {
    return UnsignedBig<0>{};
  }

  else {
    // a - b <= a for unsigned values, so the result can never need more bits than lhs;
    // underflow wraps modulo 2^lhsBits like builtin unsigned arithmetic
    using ResType = UnsignedBig<lhsBits>;
    using LhsType = UnsignedBig<lhsBits>;
    using RhsType = UnsignedBig<rhsBits>;

    auto difference = ResType{};

    if constexpr (!ResType::gIsArrayData && !RhsType::gIsArrayData) {
      difference.mData = static_cast<ResType::Data>(static_cast<ResType::Data>(lhs.mData) - static_cast<ResType::Data>(rhs.mData));

    } else {
      auto borrow = Element{ 0 }; // NOLINT(*-identifier-length)
      auto i      = Size{ 0 };    // NOLINT(*-identifier-length)

      // loop common elements for all
      for (; i < std::min(LhsType::gNumElements, RhsType::gNumElements); ++i) {
        auto const differenceElement = static_cast<Wide>(lhs.element(i)) - rhs.element(i) - borrow;
        difference.element(i, loHalf(differenceElement));
        borrow = hiHalf(differenceElement) & 1U;
      }
      // lhs tail for diff, rhs reads as zero
      for (; i < LhsType::gNumElements; ++i) {
        auto const differenceElement = static_cast<Wide>(lhs.element(i)) - borrow;
        difference.element(i, loHalf(differenceElement));
        borrow = hiHalf(differenceElement) & 1U;
      }
      // any rhs tail beyond lhs is discarded: the result wraps modulo 2^lhsBits
    }
    return difference;
  }
}

template<Size lhsBits, Size rhsBits>
constexpr auto
operator*(UnsignedBig<lhsBits> const& lhs, UnsignedBig<rhsBits> const& rhs)
{
  if constexpr (UnsignedBig<lhsBits>::gIsZeroData || UnsignedBig<rhsBits>::gIsZeroData) {
    return UnsignedBig<0>{};
  }

  else {
    constexpr auto resBits = (lhsBits == 1 ? 0 : lhsBits) + (rhsBits == 1 ? 0 : rhsBits);
    using ResType          = UnsignedBig<resBits>;
    using LhsType          = UnsignedBig<lhsBits>;
    using RhsType          = UnsignedBig<rhsBits>;

    auto product = ResType{};

    if constexpr (!ResType::gIsArrayData) {
      product.mData = static_cast<ResType::Data>(static_cast<ResType::Data>(lhs.mData) * rhs.mData);

    } else {
      // NOLINTBEGIN(*-pointer-arithmetic)
      constexpr auto lhsN     = LhsType::gNumElements;
      constexpr auto rhsN     = RhsType::gNumElements;
      constexpr auto productN = lhsN + rhsN;
      constexpr auto scratchN = karatsuba::gScratchSize<productN>;

      // one up-front stack allocation holding operands, result and all scratch;
      // mulRec picks schoolbook or sub-quadratic O(N^1.58) karatsuba per size at compile time
      auto buffer = Elements<productN * 2 + scratchN>{};

      auto* pLhsBuffer = buffer.data();
      for (Size i = 0; i < lhsN; ++i) {
        pLhsBuffer[i] = lhs.element(i);
      }

      auto* pRhsBuffer = pLhsBuffer + lhsN;
      for (Size i = 0; i < rhsN; ++i) {
        pRhsBuffer[i] = rhs.element(i);
      }

      auto* pProductBuffer = pRhsBuffer + rhsN;
      karatsuba::mulRec<lhsN, rhsN>(pLhsBuffer, pRhsBuffer, pProductBuffer, pProductBuffer + productN);

      for (Size i = 0; i < ResType::gNumElements; ++i) {
        product.element(i, pProductBuffer[i]);
      }

      // NOLINTEND(*-pointer-arithmetic)
    }
    return product;
  }
}

template<typename Rhs>
  requires(!IsUnsignedBig<Rhs>) && requires(Rhs const& rhs) { UnsignedBig{ rhs }; }
constexpr auto
operator+(IsUnsignedBig auto const& lhs, Rhs const& rhs)
{
  return lhs + UnsignedBig{ rhs };
}

template<typename Lhs>
  requires(!IsUnsignedBig<Lhs>) && requires(Lhs const& lhs) { UnsignedBig{ lhs }; }
constexpr auto
operator+(Lhs const& lhs, IsUnsignedBig auto const& rhs)
{
  return UnsignedBig{ lhs } + rhs;
}

template<typename Rhs>
  requires(!IsUnsignedBig<Rhs>) && requires(Rhs const& rhs) { UnsignedBig{ rhs }; }
constexpr auto
operator-(IsUnsignedBig auto const& lhs, Rhs const& rhs)
{
  return lhs - UnsignedBig{ rhs };
}

template<typename Lhs>
  requires(!IsUnsignedBig<Lhs>) && requires(Lhs const& lhs) { UnsignedBig{ lhs }; }
constexpr auto
operator-(Lhs const& lhs, IsUnsignedBig auto const& rhs)
{
  return UnsignedBig{ lhs } - rhs;
}

template<typename Rhs>
  requires(!IsUnsignedBig<Rhs>) && requires(Rhs const& rhs) { UnsignedBig{ rhs }; }
constexpr auto
operator*(IsUnsignedBig auto const& lhs, Rhs const& rhs)
{
  return lhs * UnsignedBig{ rhs };
}

template<typename Lhs>
  requires(!IsUnsignedBig<Lhs>) && requires(Lhs const& lhs) { UnsignedBig{ lhs }; }
constexpr auto
operator*(Lhs const& lhs, IsUnsignedBig auto const& rhs)
{
  return UnsignedBig{ lhs } * rhs;
}

} // namespace big
