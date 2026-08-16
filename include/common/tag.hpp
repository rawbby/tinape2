#pragma once

// IWYU pragma: private, include <common.hpp>

// Tag / gTag Idiom
//
// What:
// Wraps compile-time non-type values into a distinct, zero-overhead type.
//
// Why:
// Transforms compile-time values into types so they can be passed as
// function arguments. This is primarily used to resolve constructor
// ambiguities and power Class Template Argument Deduction (CTAD) for
// non-type template parameters that cannot be deduced from normal values.
//
// Example:
// Using Tag in a constructor and a deduction guide
//
// ```
// template<int Mode, typename T>
// struct Container {
//   T value;
//   constexpr Container(Tag<Mode> /* tag */, T const& val) : value(val) {}
// };
// ```
//
// Deduction Guide:
// When the compiler sees 'Tag<Mode>' and a value 'T', it uses this rule
// to deduce the full type 'Container<Mode, T>'.
//
// ```
// template<int Mode, typename T>
// Container(Tag<Mode>, T const&) -> Container<Mode, T>;
// ```
//
// Usage:
// By passing 'gTag<1>', the deduction guide infers Mode = 1.
// By passing '3.14', it infers T = double.
// The compiler automatically resolves 'c' as Container<1, double>.
//
// ```
// auto c = Container(gTag<1>, 3.14);
// ```

template<auto... /*values*/>
struct Tag
{};

template<auto... values>
constexpr auto gTag = Tag<values...>{};
