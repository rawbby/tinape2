#include <common.hpp>
#include <restricted.hpp>
#include <signed.hpp>
#include <unsigned.hpp>

#include <cstdlib>
#include <exception>
#include <type_traits>

// NOLINTBEGIN(*-magic-numbers)

namespace {

using namespace big;

void
testDeductionFromRestrictedBig()
{
  using R                      = RestrictedBig<gSignedBig<-5>, gSignedBig<10>>;
  constexpr auto restrictedVal = R{ 5 };
  constexpr auto deduced       = RestrictedBig{ restrictedVal };
  static_assert(std::is_same_v<decltype(deduced), R const>);
  static_assert(deduced.absolute() == 5);
}

void
testDeductionFromSignedBig()
{
  constexpr auto pos        = SignedBig{ gTag<Positive>, 10 };
  constexpr auto deducedPos = RestrictedBig{ pos };
  static_assert(decltype(deducedPos)::gLb == 0);
  static_assert(decltype(deducedPos)::gUb == SignedBig<Positive, decltype(pos)::gBits>{ UnsignedBig<decltype(pos)::gBits>::max() });

  constexpr auto neg        = SignedBig{ gTag<Negative>, 10 };
  constexpr auto deducedNeg = RestrictedBig{ neg };
  static_assert(decltype(deducedNeg)::gLb == SignedBig<Negative, decltype(neg)::gBits>{ gTag<Negative>, UnsignedBig<decltype(neg)::gBits>::max() });
  static_assert(decltype(deducedNeg)::gUb == 0);

  constexpr auto runtimeVal     = SignedBig{ 10 };
  constexpr auto deducedRuntime = RestrictedBig{ runtimeVal };
  static_assert(decltype(deducedRuntime)::gLb == SignedBig<Negative, decltype(runtimeVal)::gBits>{ gTag<Negative>, UnsignedBig<decltype(runtimeVal)::gBits>::max() });
  static_assert(decltype(deducedRuntime)::gUb == SignedBig<Positive, decltype(runtimeVal)::gBits>{ gTag<Positive>, UnsignedBig<decltype(runtimeVal)::gBits>::max() });
}

void
testDeductionFromUnsignedBig()
{
  constexpr auto unsignedVal = UnsignedBig<8>{ 42U };
  constexpr auto deduced     = RestrictedBig{ unsignedVal };
  static_assert(decltype(deduced)::gLb == 0);
  static_assert(decltype(deduced)::gUb == 255);
}

void
testDeductionFromIntegrals()
{
  constexpr auto u8Val     = static_cast<U8>(15U);
  constexpr auto deducedU8 = RestrictedBig{ u8Val };
  static_assert(decltype(deducedU8)::gLb == 0);
  static_assert(decltype(deducedU8)::gUb == 255);

  constexpr auto i8Val     = static_cast<I8>(-7);
  constexpr auto deducedI8 = RestrictedBig{ i8Val };
  static_assert(decltype(deducedI8)::gLb == -127);
  static_assert(decltype(deducedI8)::gUb == 127);
}

void
testDeductionDefault()
{
  constexpr auto deduced = RestrictedBig{};
  static_assert(decltype(deduced)::gLb == 0);
  static_assert(decltype(deduced)::gUb == 0);
}

} // namespace

int
main(int /* argc */, char** /* argv */)
{
  try {
    testDeductionFromRestrictedBig();
    testDeductionFromSignedBig();
    testDeductionFromUnsignedBig();
    testDeductionFromIntegrals();
    testDeductionDefault();
  } catch (std::exception const&) {
    std::exit(1); // NOLINT(*-mt-unsafe)
  }
}

// NOLINTEND(*-magic-numbers)
