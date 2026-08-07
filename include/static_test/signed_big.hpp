#pragma once

#include <big/common.hpp>          // big::SignType
#include <big/signed.hpp>          // big::SignedBig, big::gSignTag
#include <big/signed_cast.hpp>     // big::castBigSafe, big::castBigUnsafe
#include <big/signed_compare.hpp>  // operator<=>, operator==
#include <big/signed_constant.hpp> //
#include <big/unsigned.hpp>        // big::UnsignedBig

#include <type_traits> // std::is_same_v, std::is_constructible_v

namespace big::static_test {
// NOLINTBEGIN(*-magic-numbers,*-redundant-parentheses)

// clang-format off
// bits deduction: from (UnsignedBig, bool) for Runtime, via SignTag for fixed signs

static_assert(std::same_as<decltype(GenericSigned{UnsignedBig<32>{}, true}), GenericSigned<SignType::Runtime, 32>>);
static_assert(std::same_as<decltype(GenericSigned{gTag<Positive>, UnsignedBig<32>{}}), GenericSigned<SignType::Positive, 32>>);
static_assert(std::same_as<decltype(GenericSigned{gTag<Negative>, UnsignedBig<64>{}}), GenericSigned<SignType::Negative, 64>>);
static_assert(std::same_as<decltype(GenericSigned{gTag<Runtime>, UnsignedBig<64>{}, false}), GenericSigned<SignType::Runtime, 64>>);

constexpr auto gPlusOne   = GenericSigned{gTag<Positive>, UnsignedBig<32>{1}};
constexpr auto gMinusOne  = GenericSigned{gTag<Negative>, UnsignedBig<32>{1}};
constexpr auto gPlusTwo   = GenericSigned{UnsignedBig<64>{2}, true};
constexpr auto gMinusTwo  = GenericSigned{UnsignedBig<64>{2}, false};
constexpr auto gPlusZero  = GenericSigned{gTag<Positive>, UnsignedBig<32>{}};
constexpr auto gMinusZero = GenericSigned{gTag<Negative>, UnsignedBig<32>{}};

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
static_assert(gPlusOne == GenericSigned{UnsignedBig<64>{1}, true});
static_assert(gMinusOne == GenericSigned{UnsignedBig<64>{1}, false});
static_assert(GenericSigned<SignType::Positive, 64>{gPlusOne} == gPlusOne);
static_assert(GenericSigned<SignType::Runtime, 64>{gMinusOne} == gMinusOne);
static_assert(GenericSigned<SignType::Runtime, 32>{gPlusOne} == gPlusOne);
static_assert(!std::is_constructible_v<GenericSigned<SignType::Positive, 64>, decltype(gMinusOne)>);      // positive from negative
static_assert(!std::is_constructible_v<GenericSigned<SignType::Positive, 64>, decltype(gPlusTwo)>);       // positive from runtime
static_assert(!std::is_constructible_v<GenericSigned<SignType::Negative, 64>, decltype(gPlusOne)>);       // negative from positive

static_assert(decltype(gSignedBig<-3>)::gBits == 2);

// clang-format on

// NOLINTEND(*-magic-numbers,*-redundant-parentheses)
} // namespace big::static_test
