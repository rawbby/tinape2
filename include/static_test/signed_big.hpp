#pragma once

#include <big/common.hpp>         // big::SignType
#include <big/signed.hpp>         // big::SignedBig, big::gSignTag
#include <big/signed_cast.hpp>    // big::castBigSafe, big::castBigUnsafe
#include <big/signed_compare.hpp> // operator<=>, operator==
#include <big/unsigned.hpp>       // big::UnsignedBig

#include <type_traits> // std::is_same_v, std::is_constructible_v

namespace big::static_test {
// NOLINTBEGIN(*-magic-numbers,*-redundant-parentheses)

// clang-format off
// bits deduction: from (UnsignedBig, bool) for Runtime, via SignTag for fixed signs
static_assert(std::is_same_v<decltype(SignedBig{UnsignedBig<32>{1}, true}), SignedBig<SignType::Runtime, 32>>);
static_assert(std::is_same_v<decltype(SignedBig{gSignTag<SignType::Positive>, UnsignedBig<32>{1}}), SignedBig<SignType::Positive, 32>>);
static_assert(std::is_same_v<decltype(SignedBig{gSignTag<SignType::Negative>, UnsignedBig<64>{1}}), SignedBig<SignType::Negative, 64>>);
static_assert(std::is_same_v<decltype(SignedBig{gSignTag<SignType::Runtime>, UnsignedBig<64>{1}, false}), SignedBig<SignType::Runtime, 64>>);
constexpr auto gPlusOne   = SignedBig{gSignTag<SignType::Positive>, UnsignedBig<32>{1}};
constexpr auto gMinusOne  = SignedBig{gSignTag<SignType::Negative>, UnsignedBig<32>{1}};
constexpr auto gPlusTwo   = SignedBig{UnsignedBig<64>{2}, true};
constexpr auto gMinusTwo  = SignedBig{UnsignedBig<64>{2}, false};
constexpr auto gPlusZero  = SignedBig{gSignTag<SignType::Positive>, UnsignedBig<32>{}};
constexpr auto gMinusZero = SignedBig{gSignTag<SignType::Negative>, UnsignedBig<32>{}};
// comparison: <=> plus rewritten ==, !=, <, >, <=, >=
static_assert(gPlusZero == gMinusZero);
static_assert(gMinusOne < gPlusOne);
static_assert(gPlusOne > gMinusOne);
static_assert(gMinusTwo < gMinusOne);
static_assert(gMinusOne <= gMinusOne);
static_assert(gPlusOne < gPlusTwo);
static_assert(gPlusTwo >= gPlusTwo);
static_assert(gMinusOne != gPlusOne);
static_assert(gMinusOne < gPlusZero);
static_assert(gPlusZero < gPlusOne);
static_assert(gPlusZero > gMinusOne);
static_assert(gPlusOne == SignedBig{UnsignedBig<64>{1}, true});
static_assert(gMinusOne == SignedBig{UnsignedBig<64>{1}, false});
// safe converting constructor: enough bits and compatible sign only
static_assert(SignedBig<SignType::Positive, 64>{gPlusOne} == gPlusOne);
static_assert(SignedBig<SignType::Runtime, 64>{gMinusOne} == gMinusOne);
static_assert(SignedBig<SignType::Runtime, 32>{gPlusOne} == gPlusOne);
static_assert(!std::is_constructible_v<SignedBig<SignType::Positive, 64>, decltype(gMinusOne)>);      // positive from negative
static_assert(!std::is_constructible_v<SignedBig<SignType::Positive, 64>, decltype(gPlusTwo)>);       // positive from runtime
static_assert(!std::is_constructible_v<SignedBig<SignType::Negative, 64>, decltype(gPlusOne)>);       // negative from positive
static_assert(!std::is_constructible_v<SignedBig<SignType::Positive, 16>, decltype(gPlusOne)>);       // less bits from more bits
static_assert(!std::is_constructible_v<SignedBig<SignType::Runtime, 32>, decltype(gPlusTwo)>);        // less bits from more bits
// clang-format on

// NOLINTEND(*-magic-numbers,*-redundant-parentheses)
} // namespace big::static_test
