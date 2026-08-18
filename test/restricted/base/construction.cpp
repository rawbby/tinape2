#include <common.hpp>
#include <restricted.hpp>

#include <array>
#include <cstdlib>
#include <exception>

// NOLINTBEGIN(*-magic-numbers)

namespace {

using namespace big;

void
testConstruction()
{
}

} // namespace

int
main(int /* argc */, char** /* argv */)
{
  try {
    testConstruction();
  } catch (std::exception const&) {
    std::exit(1); // NOLINT(*-mt-unsafe)
  }
}

// NOLINTEND(*-magic-numbers)
