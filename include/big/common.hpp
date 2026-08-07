#pragma once

#include <common/int.hpp>
#include <common/mask.hpp>
#include <common/tag.hpp>
#include <common/type_traits.hpp>

#include <concepts>    // std::same_as
#include <cstddef>     // std::ptrdiff_t
#include <cstdint>     // std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t, std::int8_t, std::int16_t, std::int32_t, std::int64_t
#include <cstdlib>     // std::size_t
#include <type_traits> // std::remove_cvref_t

namespace big {

using Element = std::conditional_t<gU128Supported, U64, U32>;
using Wide    = std::conditional_t<gU128Supported, U128, U64>;
using Limb    = U32;

constexpr auto gElementBytes = gBytesOf<Element>;
constexpr auto gWideBytes    = gBytesOf<Wide>;
constexpr auto gLimbBytes    = gBytesOf<Limb>;

constexpr auto gElementBits = gBitsOf<Element>;
constexpr auto gWideBits    = gBitsOf<Wide>;
constexpr auto gLimbBits    = gBitsOf<Limb>;

// clang-format off
template<typename T> concept IsElement = Is<T, Element>;
template<typename T> concept IsWide    = Is<T, Wide>;
template<typename T> concept IsLimb    = Is<T, Limb>;

template<Size size> using Elements = std::array<Element, size>;
template<Size size> using Limbs    = std::array<Limb, size>;
// clang-format on

constexpr auto gLimbIsElement = IsLimb<Element>;

// NOLINTNEXTLINE(*-use-enum-class)
enum SignType : I8
{
  Negative = 0,
  Positive = 1,
  Runtime  = 2,
};

} // namespace big
