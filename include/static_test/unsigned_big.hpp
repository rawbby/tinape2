#pragma once

#include <big/common.hpp>              // big::gBigElementBits, big::gHasInt128, big::BigElement, big::BigWide
#include <big/unsigned.hpp>            // big::UnsignedBig
#include <big/unsigned_arithmetic.hpp> // operator*, operator-, operator+
#include <big/unsigned_cast.hpp>       // big::castBigSafe, big::castBigUnsafe
#include <big/unsigned_compare.hpp>    // operator<=>, operator==

#include <big/restricted.hpp>

#include <array>       // std::array
#include <type_traits> // std::is_same_v, std::is_constructible_v

namespace big::static_test {
// NOLINTBEGIN(*-magic-numbers,*-unchecked-container-access,*-redundant-parentheses)
static_assert([]() -> bool {


  return true;
}());
// NOLINTEND(*-magic-numbers,*-unchecked-container-access,*-redundant-parentheses)
} // namespace big::static_test
