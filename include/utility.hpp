#pragma once

#include <type_traits>

namespace helper {
template<typename T, template<typename...> class U>
inline constexpr bool gIsSpecializationOf = false;

template<template<typename...> class U, typename... Args>
inline constexpr bool gIsSpecializationOf<U<Args...>, U> = true;
} // namespace helper

template<typename T, template<typename...> class U>
concept SpecializationOf = helper::gIsSpecializationOf<std::remove_cvref_t<T>, U>;
