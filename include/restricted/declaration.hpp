#pragma once

// IWYU pragma: private, include <restricted.hpp>

#include <signed.hpp>

namespace big {

// using lb one can save representation bits by finding the unused lower bits
// e.g.:
// lb: 0b00001100
// rb: 0b00110000
// in total all information can be stored using 4bits
// for an underlying integer type saving these bits doesn't make much sense
// shifting values is just overhead that is complex and does not store anything real.
// for underlying array type (real big types) this looks different. with shifting the
// result might even be able fit an underlying int type again.

// so the logic looks like this:
// - (lb::bits <= gWideBits) and (ub <= gWideBits) -> store and handle naiive
// - (lb.sign() == ub.sign()) and (ub::bits - lb::bits <= gWideBits) -> store and handle (naiive + exact shift)
// - else -> store and handle (elements + element shift)
// todo: evaluate if the element shift results in an effective reuse the karatsuba implementation for multiplication

template<IsSignedBig auto lb, IsSignedBig auto ub>
struct RestrictedBig
{

};

} // namespace big
