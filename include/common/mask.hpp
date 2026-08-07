#pragma once

#include <common/int.hpp>

[[nodiscard]] constexpr auto
loHalf(
  IsUnsignedIntegral auto value)
{
  constexpr auto halfBits = gBitsOf<decltype(value)> / 2;
  return static_cast<U<halfBits>>(value);
}

[[nodiscard]] constexpr auto
hiHalf(
  IsUnsignedIntegral auto value)
{
  constexpr auto halfBits = gBitsOf<decltype(value)> / 2;
  return static_cast<U<halfBits>>(value >> halfBits);
}

template<IsUnsignedIntegral T>
constexpr auto gMask = static_cast<T>(-1);

template<IsUnsignedIntegral T>
[[nodiscard]] constexpr auto
loMask(
  Size bits) -> T
{
  if (bits >= gBitsOf<T>) {
    return gMask<T>;
  }
  return static_cast<T>(static_cast<T>(T{ 1 } << bits) - T{ 1 });
}

template<IsUnsignedIntegral T>
[[nodiscard]] constexpr auto
hiMask(
  Size bits) -> T
{
  if (bits >= gBitsOf<T>) {
    return gMask<T>;
  }
  return static_cast<T>(gMask<T> ^ static_cast<T>(gMask<T> >> bits));
}

template<typename T, Size bits>
constexpr auto gLoMask = loMask<T>(bits);

template<typename T, Size bits>
constexpr auto gHiMask = hiMask<T>(bits);
