#include <common.hpp>
#include <unsigned.hpp>

#include <array>
#include <cstdlib>
#include <exception>

// NOLINTBEGIN(*-magic-numbers)

namespace {

using namespace big;

/// element() must return the stored elements of array data
void
testElementAccessOnArrayData()
{
  constexpr auto value = UnsignedBig{ std::array{
    static_cast<Element>(1),
    static_cast<Element>(2),
    static_cast<Element>(3),
  } };
  static_assert(value.element(0) == static_cast<Element>(1));
  static_assert(value.element(1) == static_cast<Element>(2));
  static_assert(value.element(2) == static_cast<Element>(3));
}

/// element() must mirror the underlying data representation
void
testElementMatchesUnderlyingData()
{
  constexpr auto value = UnsignedBig{ std::array{
    static_cast<Element>(7),
    static_cast<Element>(11),
  } };
  static_assert(value.element(0) == value.mData[0]);
  static_assert(value.element(1) == value.mData[1]);
}

/// element() of a zero value must be zero everywhere
void
testElementOfZero()
{
  constexpr auto value = UnsignedBig{ std::array{
    static_cast<Element>(0),
    static_cast<Element>(0),
    static_cast<Element>(0),
  } };
  static_assert(value.element(0) == static_cast<Element>(0));
  static_assert(value.element(1) == static_cast<Element>(0));
  static_assert(value.element(2) == static_cast<Element>(0));
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
