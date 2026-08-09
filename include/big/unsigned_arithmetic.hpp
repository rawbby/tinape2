#pragma once

#include <big/common.hpp>               // big::BigElement, big::BigWide, big::gBigElementBits, big::loBigElement, big::hiBigElement
#include <big/unsigned.hpp>             // big::UnsignedBig
#include <big/unsigned_karatsuba.hpp>   // big::karatsuba::gScratchSize, big::karatsuba::mulRec
#include <big/unsigned_type_traits.hpp> // big::IsUnsignedBig

#include <algorithm> // std::max, std::min
#include <array>     // std::array
#include <stdexcept> // std::invalid_argument

namespace big {

template<Size lhsBits, Size rhsBits>
constexpr auto
operator+(UnsignedBig<lhsBits> const& lhs, UnsignedBig<rhsBits> const& rhs)
{
  // ensure bigger num gets swapped into lhs
  if constexpr (rhsBits > lhsBits) {
    return rhs + lhs;

  } else {
    constexpr auto resBits = rhsBits > 0 ? lhsBits + 1 : lhsBits;
    using ResType          = UnsignedBig<resBits>;
    using LhsType          = UnsignedBig<lhsBits>;
    using RhsType          = UnsignedBig<rhsBits>;

    if constexpr (!ResType::gIsArrayData) {
      return ResType{ static_cast<ResType::Data>(lhs.mData) + rhs.mData };

    } else {
      // NOLINTBEGIN(*-magic-numbers,*-identifier-length,*-unchecked-container-access)

      auto sum   = ResType{};
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

      return sum;
      // NOLINTEND(*-magic-numbers,*-identifier-length,*-unchecked-container-access)
    }
  }
}

template<Size lhsBits, Size rhsBits>
constexpr auto
operator-(UnsignedBig<lhsBits> const& lhs, UnsignedBig<rhsBits> const& rhs)
{
  if (std::is_constant_evaluated()) {
    if (rhs > lhs) {
      throw std::invalid_argument{ "undefined behaviour" };
    }
  }

  // a - b <= a for unsigned values, so the result can never need more bits than lhs;
  // underflow wraps modulo 2^lhsBits like builtin unsigned arithmetic
  using ResType = UnsignedBig<lhsBits>;
  using LhsType = UnsignedBig<lhsBits>;
  using RhsType = UnsignedBig<rhsBits>;

  if constexpr (!ResType::gIsArrayData && !RhsType::gIsArrayData) {
    return ResType{ static_cast<ResType::Data>(static_cast<ResType::Data>(lhs.mData) - static_cast<ResType::Data>(rhs.mData)) };

  } else {
    // NOLINTBEGIN(*-magic-numbers,*-identifier-length,*-unchecked-container-access)
    auto difference = ResType{};
    auto borrow     = Element{ 0 };
    auto i          = Size{ 0 };

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

    return difference;
    // NOLINTEND(*-magic-numbers,*-identifier-length,*-unchecked-container-access)
  }
}

template<Size lhsBits, Size rhsBits>
constexpr auto
operator*(UnsignedBig<lhsBits> const& lhs, UnsignedBig<rhsBits> const& rhs)
{
  constexpr auto resBits = lhsBits == 0 || rhsBits == 0 ? 0 : (lhsBits == 1 ? 0 : lhsBits) + (rhsBits == 1 ? 0 : rhsBits);
  using ResType          = UnsignedBig<resBits>;
  using LhsType          = UnsignedBig<lhsBits>;
  using RhsType          = UnsignedBig<rhsBits>;

  if constexpr (!ResType::gIsArrayData) {
    return ResType{ static_cast<ResType::Data>(lhs.mData) * rhs.mData };

  } else {
    // NOLINTBEGIN(*-identifier-length,*-pointer-arithmetic,*-braces-around-statements)
    constexpr auto lhsN     = LhsType::gNumElements;
    constexpr auto rhsN     = RhsType::gNumElements;
    constexpr auto resN     = ResType::gNumElements;
    constexpr auto n        = lhsN + rhsN;
    constexpr auto scratchN = karatsuba::gScratchSize<n>;

    // one up-front stack allocation holding operands, result and all scratch;
    // mulRec picks schoolbook or sub-quadratic O(N^1.58) karatsuba per size at compile time
    auto buffer = std::array<Element, n * 2 + scratchN>{};

    auto* a = buffer.data();
    for (Size i = 0; i < lhsN; ++i)
      a[i] = lhs.element(i);

    auto* b = a + lhsN;
    for (Size i = 0; i < rhsN; ++i)
      b[i] = rhs.element(i);

    auto* res = b + rhsN;
    karatsuba::mulRec<lhsN, rhsN>(a, b, res, res + n);

    auto product = ResType{};
    for (Size i = 0; i < resN; ++i)
      product.element(i, res[i]);

    return product;
    // NOLINTEND(*-identifier-length,*-pointer-arithmetic,*-braces-around-statements)
  }
}

// NOLINTBEGIN(*-macro-usage)
// clang-format off
#ifdef BIG_MAKE_WRAPPER
#error "BIG_MAKE_WRAPPER is already defined"
#endif
#define BIG_MAKE_WRAPPER(OP)                                                     \
  template<typename Rhs>                                                         \
    requires(!IsUnsignedBig<Rhs>) && requires(Rhs const& rhs){UnsignedBig{rhs};} \
  constexpr auto operator OP(IsUnsignedBig auto const& lhs, Rhs const& rhs)      \
  {                                                                              \
    return lhs OP UnsignedBig{ rhs };                                            \
  }                                                                              \
                                                                                 \
  template<typename Lhs>                                                         \
    requires(!IsUnsignedBig<Lhs>) && requires(Lhs const& lhs){UnsignedBig{lhs};} \
  constexpr auto operator OP(Lhs const& lhs, IsUnsignedBig auto const& rhs)      \
  {                                                                              \
    return UnsignedBig{ lhs } OP rhs;                                            \
  }
BIG_MAKE_WRAPPER(+)
BIG_MAKE_WRAPPER(-)
BIG_MAKE_WRAPPER(*)
#undef BIG_MAKE_WRAPPER
// clang-format on
// NOLINTEND(*-macro-usage)

} // namespace big
