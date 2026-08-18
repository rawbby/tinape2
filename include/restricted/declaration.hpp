#pragma once

// IWYU pragma: private, include <restricted.hpp>

#include <restricted/forward.hpp>
#include <signed.hpp>

namespace big {

template<IsSignedBig auto lb, IsSignedBig auto ub>
struct RestrictedBig
{
  using Traits   = impl::IsRestrictedBigHelper<lb, ub>;
  using Storage  = typename Traits::Storage;
  using Absolute = SignedBig<Traits::gSignType, Traits::gMagBits>;

  constexpr static auto gLb       = lb;
  constexpr static auto gUb       = ub;
  constexpr static auto gSignType = Traits::gSignType;
  constexpr static auto gMode     = Traits::gMode;
  constexpr static auto gBits     = Storage::gBits;

  [[no_unique_address]] Storage mStorage{};
};

} // namespace big
