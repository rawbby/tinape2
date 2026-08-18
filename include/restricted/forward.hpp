#pragma once

// IWYU pragma: private, include <restricted.hpp>

#include <signed.hpp>

namespace big {

template<IsSignedBig auto lb, IsSignedBig auto ub>
struct RestrictedBig;

} // namespace big
