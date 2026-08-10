#pragma once

// IWYU pragma: private, include <common.hpp>

#include <array>
#include <concepts>
#include <type_traits>
#include <vector>

template<typename Lhs, typename Rhs>
concept Is = std::same_as<std::remove_cvref_t<Lhs>, std::remove_cvref_t<Rhs>>;

template<typename T, typename... Ts>
concept IsAny = (Is<T, Ts> || ...);

template<typename Array>
concept IsArray = requires { []<typename T, auto N>(std::type_identity<std::array<T, N>>) {}(std::type_identity<std::remove_cvref_t<Array>>{}); };

template<typename Array, typename T>
concept IsArrayOf = requires { []<auto N>(std::type_identity<std::array<T, N>>) {}(std::type_identity<std::remove_cvref_t<Array>>{}); };
