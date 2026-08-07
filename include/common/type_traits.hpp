#pragma once

#include <concepts>    // std::same_as
#include <tuple>       // std::tuple_element_t, std::tuple
#include <type_traits> // std::remove_cvref_t
#include <utility>     // std::index_sequence, std::make_index_sequence

template<typename Lhs, typename Rhs>
concept Is = std::same_as<std::remove_cvref_t<Lhs>, std::remove_cvref_t<Rhs>>;

template<typename T, typename... Ts>
concept IsAny = (Is<T, Ts> || ...);
