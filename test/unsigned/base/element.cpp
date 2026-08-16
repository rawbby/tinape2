#include <unsigned.hpp>

#include <cstdlib>
#include <exception>

// NOLINTBEGIN(*-magic-numbers)

namespace {

using namespace big;

/// element() must return the stored elements of array data
void
testElementAccessOnArrayData()
{
  constexpr auto value = UnsignedBig{ Elements<3>{ 1, 2, 3 } };
  static_assert(value.element(0) == 1);
  static_assert(value.element(1) == 2);
  static_assert(value.element(2) == 3);
}

/// element() must mirror the underlying data representation
void
testElementMatchesUnderlyingData()
{
  constexpr auto value = UnsignedBig{ Limbs<5>{ 1, 2, 3, 4, 5 } };
  static_assert(value.limb(0) == 1);
  static_assert(value.limb(1) == 2);
  static_assert(value.limb(2) == 3);
  static_assert(value.limb(3) == 4);
  static_assert(value.limb(4) == 5);
}

/// element() of a zero value must be zero everywhere
void
testElementOfZero()
{
  constexpr auto value = UnsignedBig{ Elements<3>{ 0, 0, 0 } };
  static_assert(value.element(0) == 0);
  static_assert(value.element(1) == 0);
  static_assert(value.element(2) == 0);
}

} // namespace

int
main(int /* argc */, char** /* argv */)
{
  try {
    testElementAccessOnArrayData();
    testElementMatchesUnderlyingData();
    testElementOfZero();
  } catch (std::exception const&) {
    std::exit(1); // NOLINT(*-mt-unsafe)
  }
}

// NOLINTEND(*-magic-numbers)
