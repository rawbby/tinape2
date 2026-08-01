#pragma once

#include <big/unsigned.hpp> // big::UnsignedBig

#include <algorithm> // std::max
#include <cstddef>   // std::size_t

namespace big {

template<typename T>
inline constexpr bool gIsUnsignedBig = false;

template<Size bits>
inline constexpr bool gIsUnsignedBig<UnsignedBig<bits>> = true;

template<typename T>
concept IsUnsignedBig = gIsUnsignedBig<T>;

template<IsUnsignedBig... Args>
using CommonUnsignedBig = UnsignedBig<std::max({ Args::gBits... })>;

} // namespace big
