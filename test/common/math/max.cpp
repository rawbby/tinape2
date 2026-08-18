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
  static_assert(gMax<42> == 42);
  static_assert(std::is_same_v<decltype(gMax<42>), int const>);
}

void
testMultipleArgs()
{
  static_assert(gMax<10, 3, 7, 2, 9> == 10);
  static_assert(gMax<-5, -10, 0, 5> == 5);
}

void
testTypePreservation()
{
  constexpr auto valU10 = 10U;
  constexpr auto valS5  = 5LL;
  constexpr auto valU2  = 2U;

  static_assert(std::is_same_v<decltype(gMax<valU10, valS5>), unsigned int const>);
  static_assert(gMax<valU10, valS5> == 10U);

  static_assert(std::is_same_v<decltype(gMax<valU2, valS5>), long long const>);
  static_assert(gMax<valU2, valS5> == 5LL);
}

void
testBigTypes()
{
  constexpr auto valA = SignedBig<Positive, 32>{ 10U };
  constexpr auto valB = SignedBig<Runtime, 64>{ -20 };
  constexpr auto valC = SignedBig<Positive, 64>{ 50U };

  static_assert(std::is_same_v<decltype(gMax<valA, valB>), SignedBig<Positive, 32> const>);
  static_assert(gMax<valA, valB> == 10);

  static_assert(std::is_same_v<decltype(gMax<valA, valC>), SignedBig<Positive, 64> const>);
  static_assert(gMax<valA, valC> == 50);

  static_assert(gMax<gSignedBig<100>, gSignedBig<20>, gSignedBig<50>> == 100);
}

void
testRuntimeVerification()
{
  require(gMax<10, 5, 20> == 20);
  require(gMax<gSignedBig<10>, gSignedBig<-10>> == 10);
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
