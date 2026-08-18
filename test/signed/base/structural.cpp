#include <signed.hpp>
#include <unsigned.hpp>

#include <cstdlib>
#include <exception>
#include <type_traits>

// NOLINTBEGIN(*-magic-numbers)

namespace {

using namespace big;

template<IsSignedBig auto Value>
struct StructuralHolder
{
  constexpr static auto gValue = Value;
};

template<IsSignedBig auto Lb, IsSignedBig auto Ub>
struct StructuralInterval
{
  constexpr static auto gLb = Lb;
  constexpr static auto gUb = Ub;
};

void
require(bool condition)
{
  if (!condition) {
    std::abort();
  }
}

void
testStructuralTypeUsageAsNttp()
{
  // zero value
  using ZeroHolder = StructuralHolder<gSignedBig<0>>;
  static_assert(ZeroHolder::gValue == 0);
  static_assert(ZeroHolder::gValue.abs() == 0U);

  // negative narrow value
  using NegFiveHolder = StructuralHolder<gSignedBig<-5>>;
  static_assert(NegFiveHolder::gValue == -5);
  static_assert(!NegFiveHolder::gValue.sign());
  static_assert(NegFiveHolder::gValue.abs() == 5U);

  // positive narrow value
  using PosTenHolder = StructuralHolder<gSignedBig<10>>;
  static_assert(PosTenHolder::gValue == 10);
  static_assert(PosTenHolder::gValue.sign());
  static_assert(PosTenHolder::gValue.abs() == 10U);

  // multi-element positive value
  constexpr auto multiPos = SignedBig<Positive, 3 * gElementBits>{ Elements<3>{ 1, 2, 3 } };
  using MultiPosHolder    = StructuralHolder<multiPos>;
  static_assert(MultiPosHolder::gValue.sign());
  static_assert(MultiPosHolder::gValue.abs().element(0) == 1);
  static_assert(MultiPosHolder::gValue.abs().element(1) == 2);
  static_assert(MultiPosHolder::gValue.abs().element(2) == 3);

  // multi-element negative value
  constexpr auto multiNeg = SignedBig<Negative, 3 * gElementBits>{ Elements<3>{ 4, 5, 6 } };
  using MultiNegHolder    = StructuralHolder<multiNeg>;
  static_assert(!MultiNegHolder::gValue.sign());
  static_assert(MultiNegHolder::gValue.abs().element(0) == 4);
  static_assert(MultiNegHolder::gValue.abs().element(1) == 5);
  static_assert(MultiNegHolder::gValue.abs().element(2) == 6);

  // multi-element runtime-sign value
  constexpr auto multiRuntime = SignedBig<Runtime, 3 * gElementBits>{ Elements<3>{ 7, 8, 9 }, false };
  using MultiRuntimeHolder    = StructuralHolder<multiRuntime>;
  static_assert(!MultiRuntimeHolder::gValue.sign());
  static_assert(MultiRuntimeHolder::gValue.abs().element(0) == 7);
  static_assert(MultiRuntimeHolder::gValue.abs().element(1) == 8);
  static_assert(MultiRuntimeHolder::gValue.abs().element(2) == 9);

  // interval with bounds as NTTPs
  using Interval = StructuralInterval<gSignedBig<-5>, gSignedBig<10>>;
  static_assert(Interval::gLb == -5);
  static_assert(Interval::gUb == 10);

  // template-argument equivalence
  static_assert(std::is_same_v<StructuralHolder<gSignedBig<-5>>, StructuralHolder<gSignedBig<-5>>>);
  static_assert(std::is_same_v<StructuralHolder<gSignedBig<10>>, StructuralHolder<gSignedBig<10>>>);
  static_assert(std::is_same_v<StructuralHolder<gSignedBig<-5>>, StructuralHolder<SignedBig<Negative, 3>{ 5U }>>);
  static_assert(!std::is_same_v<StructuralHolder<gSignedBig<-5>>, StructuralHolder<gSignedBig<5>>>);
}

void
testRuntimeVerification()
{
  using Interval = StructuralInterval<gSignedBig<-5>, gSignedBig<10>>;
  require(Interval::gLb == -5);
  require(Interval::gUb == 10);
  require(!Interval::gLb.sign());
  require(Interval::gUb.sign());
}

} // namespace

int
main(int /* argc */, char** /* argv */)
{
  try {
    testStructuralTypeUsageAsNttp();
    testRuntimeVerification();
  } catch (std::exception const&) {
    std::exit(1); // NOLINT(*-mt-unsafe)
  }
}

// NOLINTEND(*-magic-numbers)
