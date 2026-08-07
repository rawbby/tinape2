#pragma once

#include <big/common.hpp>               // big::SignType
#include <big/signed.hpp>               // big::SignedBig
#include <big/unsigned_type_traits.hpp> // big::CommonUnsignedBig

#include <cstddef> // std::size_t
#include <tuple>   // std::tuple

namespace big {

template<typename T>
inline constexpr bool gIsSignedBig = false;

template<SignType signType, Size bits>
inline constexpr bool gIsSignedBig<GenericSigned<signType, bits>> = true;

template<typename T>
concept IsSignedBig = gIsSignedBig<T>;

// clang-format off
template <IsSignedBig... Ts>
using CommonSignedBig = GenericSigned<
    ((Ts::gSignType == std::tuple_element_t<0, std::tuple<Ts...>>::gSignType) && ...)
        ? std::tuple_element_t<0, std::tuple<Ts...>>::gSignType
        : SignType::Runtime,
    CommonUnsignedBig<typename Ts::UnsignedBig...>::gBits>;
// clang-format on

} // namespace big
