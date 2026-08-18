#include <common.hpp>
#include <signed.hpp>

#include <cstdlib>
#include <exception>
#include <type_traits>

// NOLINTBEGIN(*-magic-numbers)

namespace {

using namespace big;

void
require(bool condition)
{
  if (!condition) {
    std::abort();
  }
}

void
testSingleArg()
{
  static_assert(gMin<42> == 42);
  static_assert(std::is_same_v<decltype(gMin<42>), int const>);
}

void
testMultipleArgs()
{
  static_assert(gMin<10, 3, 7, 2, 9> == 2);
  static_assert(gMin<-5, -10, 0, 5> == -10);
}

void
testTypePreservation()
{
  constexpr auto valU10 = 10U;
  constexpr auto valS5  = 5LL;
  constexpr auto valU2  = 2U;

  static_assert(std::is_same_v<decltype(gMin<valU10, valS5>), long long const>);
  static_assert(gMin<valU10, valS5> == 5LL);

  static_assert(std::is_same_v<decltype(gMin<valU2, valS5>), unsigned int const>);
  static_assert(gMin<valU2, valS5> == 2U);
}

void
testBigTypes()
{
  constexpr auto valA = SignedBig<Positive, 32>{ 10U };
  constexpr auto valB = SignedBig<Runtime, 64>{ -20 };
  constexpr auto valC = SignedBig<Negative, 16>{ 5U };

  static_assert(std::is_same_v<decltype(gMin<valA, valB>), SignedBig<Runtime, 64> const>);
  static_assert(gMin<valA, valB> == -20);

  static_assert(std::is_same_v<decltype(gMin<valA, valC>), SignedBig<Negative, 16> const>);
  static_assert(gMin<valA, valC> == -5);

  static_assert(gMin<gSignedBig<100>, gSignedBig<20>, gSignedBig<50>> == 20);
}

void
testRuntimeVerification()
{
  require(gMin<10, 5, 20> == 5);
  require(gMin<gSignedBig<10>, gSignedBig<-10>> == -10);
}

} // namespace

int
main(int /* argc */, char** /* argv */)
{
  try {
    testSingleArg();
    testMultipleArgs();
    testTypePreservation();
    testBigTypes();
    testRuntimeVerification();
  } catch (std::exception const&) {
    std::exit(1); // NOLINT(*-mt-unsafe)
  }
}

// NOLINTEND(*-magic-numbers)
