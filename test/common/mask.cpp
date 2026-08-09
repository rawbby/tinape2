#include <common/int.hpp>
#include <common/mask.hpp>

#include <cstdlib>
#include <exception>

// NOLINTBEGIN(*-magic-numbers)

int
main(int /* argc */, char** /* argv */)
{
  try {
    static_assert(gLoMask<U8, 0> == 0b00000000);
    static_assert(gLoMask<U8, 3> == 0b00000111);
    static_assert(gLoMask<U8, 8> == 0b11111111);
    static_assert(gLoMask<U8, 12> == 0b11111111);
    static_assert(gHiMask<U8, 0> == 0b00000000);
    static_assert(gHiMask<U8, 3> == 0b11100000);
    static_assert(gHiMask<U8, 8> == 0b11111111);
    static_assert(gHiMask<U8, 12> == 0b11111111);
  } catch (std::exception const&) {
    std::exit(1); // NOLINT(*-mt-unsafe)
  }
}

// NOLINTEND(*-magic-numbers)
