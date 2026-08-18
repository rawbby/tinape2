#include <restricted.hpp>
#include <signed.hpp>

#include <cstdlib>
#include <exception>

// NOLINTBEGIN(*-magic-numbers)

namespace {

using namespace big;

void
testConstruction()
{
  RestrictedBig<gSignedBig<-5>, gSignedBig<10>> const restrictedVal{};
  static_cast<void>(restrictedVal);
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
