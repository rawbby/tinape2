#pragma once

// IWYU pragma: private, include <common.hpp>

template<auto first, auto... rest>
constexpr auto gMin = []() consteval -> decltype(auto) {
  if constexpr (sizeof...(rest) == 0) {
    return first;
  } else {
    constexpr auto next = gMin<rest...>;
    if constexpr (first < next) {
      return first;
    } else {
      return next;
    }
  }
}();

template<auto first, auto... rest>
constexpr auto gMax = []() consteval -> decltype(auto) {
  if constexpr (sizeof...(rest) == 0) {
    return first;
  } else {
    constexpr auto next = gMax<rest...>;
    if constexpr (first > next) {
      return first;
    } else {
      return next;
    }
  }
}();
