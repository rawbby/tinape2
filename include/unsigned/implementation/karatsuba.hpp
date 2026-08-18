#pragma once

// IWYU pragma: private, include <unsigned.hpp>

#include <unsigned/forward.hpp>

#include <common/int.hpp>

#include <algorithm>

namespace big::karatsuba {
// NOLINTBEGIN(*-magic-numbers,*-identifier-length,*-pointer-arithmetic,misc-redundant-expression)

// scratch (in elements) needed by mulRec for operands of n total elements
template<Size n>
constexpr auto gScratchSize = (n * 4) + 64;

// res[0..na+nb) = a[0..na) * b[0..nb), schoolbook
template<Size na, Size nb>
constexpr void
mulSchool(Element const* __restrict a, Element const* __restrict b, Element* __restrict res)
{
  for (Size i = 0; i < na + nb; ++i) {
    res[i] = 0;
  }
  for (Size i = 0; i < na; ++i) {
    auto carry = Wide{ 0 };
    for (Size j = 0; j < nb; ++j) {
      auto const prod = (static_cast<Wide>(a[i]) * b[j]) + res[i + j] + carry;
      res[i + j]      = static_cast<Element>(prod);
      carry           = prod >> gElementBits;
    }
    res[i + nb] = static_cast<Element>(carry);
  }
}

// res[shift..len) += src[0..srcLen), carry clamped at len
template<Size len, Size shift, Size srcLen>
constexpr void
addAt(Element* __restrict res, Element const* __restrict src)
{
  auto carry = Wide{ 0 };
  auto i     = Size{ 0 };
  for (; i < srcLen && shift + i < len; ++i) {
    auto const sum = static_cast<Wide>(res[shift + i]) + src[i] + carry;
    res[shift + i] = static_cast<Element>(sum);
    carry          = sum >> gElementBits;
  }
  for (; carry != 0 && shift + i < len; ++i) {
    auto const sum = static_cast<Wide>(res[shift + i]) + carry;
    res[shift + i] = static_cast<Element>(sum);
    carry          = sum >> gElementBits;
  }
}

// res[0..len) -= src[0..srcLen), never underflows for karatsuba middle term
template<Size len, Size srcLen>
constexpr void
subAt(Element* __restrict res, Element const* __restrict src)
{
  auto borrow = Wide{ 0 };
  auto i      = Size{ 0 };
  for (; i < srcLen && i < len; ++i) {
    auto const diff = static_cast<Wide>(res[i]) - src[i] - borrow;
    res[i]          = static_cast<Element>(diff);
    borrow          = (diff >> gElementBits) & 1U;
  }
  for (; borrow != 0 && i < len; ++i) {
    auto const diff = static_cast<Wide>(res[i]) - borrow;
    res[i]          = static_cast<Element>(diff);
    borrow          = (diff >> gElementBits) & 1U;
  }
}

// out[0..na+1) = a[0..na) + b[0..nb), requires nb <= na
template<Size na, Size nb>
constexpr void
addFull(Element const* a, Element const* b, Element* __restrict out)
{
  auto carry = Wide{ 0 };
  auto i     = Size{ 0 };
  for (; i < nb; ++i) {
    auto const sum = static_cast<Wide>(a[i]) + b[i] + carry;
    out[i]         = static_cast<Element>(sum);
    carry          = sum >> gElementBits;
  }
  for (; i < na; ++i) {
    auto const sum = static_cast<Wide>(a[i]) + carry;
    out[i]         = static_cast<Element>(sum);
    carry          = sum >> gElementBits;
  }
  out[na] = static_cast<Element>(carry);
}

// res[0..na+nb) = a[0..na) * b[0..nb); scratch provided once by the caller
template<Size na, Size nb>
constexpr void
mulRec(Element const* __restrict a, Element const* __restrict b, Element* __restrict res, Element* __restrict scratch)
{
  if constexpr (na < nb) { // keep the longer operand in a
    mulRec<nb, na>(b, a, res, scratch);

  } else if constexpr (nb <= 4) {
    mulSchool<na, nb>(a, b, res);

  } else {
    constexpr auto k  = na / 2;
    constexpr auto ha = na - k;

    if constexpr (nb <= k) { // b too short to split: res = a0*b + (a1*b << k)
      mulRec<k, nb>(a, b, res, scratch);
      for (Size i = 0; i < ha; ++i) {
        res[k + nb + i] = 0;
      }
      auto* part = scratch; // ha + nb chunks
      mulRec<ha, nb>(a + k, b, part, scratch + ha + nb);
      addAt<na + nb, k, ha + nb>(res, part);

    } else {
      constexpr auto hb    = nb - k;
      constexpr auto saLen = ha + 1;
      constexpr auto sbLen = std::max(hb, k) + 1;

      // p0 = a0*b0, p1 = a1*b1 straight into the result
      mulRec<k, k>(a, b, res, scratch);
      mulRec<ha, hb>(a + k, b + k, res + (k * 2), scratch);

      // p2 = (a0+a1)*(b0+b1) inside scratch
      auto* sa = scratch;
      auto* sb = sa + saLen;
      auto* p2 = sb + sbLen;
      addFull<ha, k>(a + k, a, sa);
      addFull<std::max(hb, k), std::min(hb, k)>(hb >= k ? b + k : b, hb >= k ? b : b + k, sb);
      mulRec<saLen, sbLen>(sa, sb, p2, p2 + saLen + sbLen);

      // p2 -= p0 + p1; res += p2 << k
      subAt<saLen + sbLen, k * 2>(p2, res);
      subAt<saLen + sbLen, ha + hb>(p2, res + (k * 2));
      addAt<na + nb, k, saLen + sbLen>(res, p2);
    }
  }
}

// NOLINTEND(*-magic-numbers,*-identifier-length,*-pointer-arithmetic,misc-redundant-expression)
} // namespace big::karatsuba
