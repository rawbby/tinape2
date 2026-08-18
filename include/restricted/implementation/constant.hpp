#pragma once

// IWYU pragma: private, include <restricted.hpp>

#include <restricted/declaration.hpp>
#include <restricted/forward.hpp>

#include <signed.hpp>

namespace big {

template<auto value>
  requires requires { gSignedBig<value>; }
constexpr auto gRestrictedBig = RestrictedBig<gSignedBig<value>, gSignedBig<value>>{ gSignedBig<value> };

template<auto lbValue, auto ubValue>
  requires requires {
    gSignedBig<lbValue>;
    gSignedBig<ubValue>;
  }
using Restricted = RestrictedBig<gSignedBig<lbValue>, gSignedBig<ubValue>>;

} // namespace big
