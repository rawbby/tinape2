#pragma once

#include <rational.hpp> // rational::val, rational::operator==, rational::operator!=, rational::operator<, rational::operator>, rational::operator<=, rational::operator>=, rational::operator+, rational::operator-, rational::operator*, rational::operator/, rational::operator<=>

#include <compare> // std::strong_ordering

namespace rational::static_test {
// NOLINTBEGIN(*-magic-numbers,*-redundant-parentheses)
using namespace ::rational;
static_assert(val(5) == val(5));
static_assert(val(5) != val(10));
static_assert(val(5) < val(10));
static_assert(val(10) > val(5));
static_assert(val(5) <= val(5));
static_assert(val(10) >= val(5));
constexpr int gNum = 3;
constexpr int gDen = 4;
static_assert(val(&gNum) == val(3));
static_assert(val(&gNum) / val(&gDen) == val(3) / val(4));
static_assert(val(1) / val(2) == val(2) / val(4));
static_assert(val(3) / val(3) == val(1));
static_assert(val(4) / val(2) == val(2));
static_assert(val(10) / val(5) == val(20) / val(10));
static_assert(val(1) / val(3) < val(1) / val(2));
static_assert(val(3) / val(4) > val(2) / val(3));
static_assert(val(1) / val(4) <= val(1) / val(4));
static_assert(val(1) / val(4) <= val(1) / val(3));
static_assert(val(5) / val(4) >= val(1) / val(1));
static_assert(val(1) / val(2) + val(1) / val(2) == val(1));
static_assert(val(1) / val(2) - val(1) / val(3) == val(1) / val(6));
static_assert(val(1) + val(1) / val(2) == val(3) / val(2));
static_assert(val(3) / val(2) - val(1) == val(1) / val(2));
static_assert((val(1) / val(2)) * (val(2) / val(3)) == val(1) / val(3));
static_assert((val(1) / val(2)) / (val(1) / val(3)) == val(3) / val(2));
static_assert((val(2) * val(3)) / val(4) == val(3) / val(2));
static_assert(((val(1) / val(2) + val(1) / val(3)) * (val(6) / val(5))) == val(1));
static_assert((val(3) / val(4) - val(1) / val(4)) / (val(1) / val(2)) == val(1));
static_assert((val(1) / val(2) <=> val(2) / val(4)) == std::strong_ordering::equal);
static_assert((val(1) / val(3) <=> val(1) / val(2)) == std::strong_ordering::less);
static_assert((val(3) / val(4) <=> val(2) / val(3)) == std::strong_ordering::greater);
static_assert(val(-1) / val(2) == val(1) / val(-2));
static_assert(val(-1) / val(2) < val(0));
static_assert(val(1) / val(-2) > val(-1) / val(1));
static_assert(val(-1) / val(2) + val(1) / val(2) == val(0));
// NOLINTEND(*-magic-numbers,*-redundant-parentheses)
} // namespace rational::static_test
