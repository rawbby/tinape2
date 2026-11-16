#pragma once

constexpr bool
operator==(IsFixed auto lhs, IsFixed auto rhs)
{
  using Lhs = FixedTraits<decltype(lhs)>;
  using Rhs = FixedTraits<decltype(rhs)>;

  // handle first var by conversion
  if constexpr (Lhs::S == VAR) {
    if (lhs.repr < 0) {
      auto tmp = Fixed<NEG, Lhs::B, Lhs::P>{};
      tmp.repr = std::abs(lhs.repr);
      return tmp == rhs;
    }
    if (lhs.repr > 0) {
      auto tmp = Fixed<POS, Lhs::B, Lhs::P>{};
      tmp.repr = std::abs(lhs.repr);
      return tmp == rhs;
    }
    return fixed_zero == rhs;
  }

  // handle second var by conversion
  if constexpr (Rhs::S == VAR) {
    if (rhs.repr < 0) {
      auto tmp = Fixed<NEG, Rhs::B, Rhs::P>{};
      tmp.repr = std::abs(rhs.repr);
      return lhs == tmp;
    }
    if (rhs.repr > 0) {
      auto tmp = Fixed<POS, Rhs::B, Rhs::P>{};
      tmp.repr = std::abs(rhs.repr);
      return lhs == tmp;
    }
    return lhs == fixed_zero;
  }

  // handle nil
  else if constexpr (Lhs::S == NIL && Rhs::S == NIL)
    return true;
  else if constexpr (Lhs::S == NIL)
    return !rhs.repr;
  else if constexpr (Rhs::S == NIL)
    return !lhs.repr;

  // handle contrary signs
  else if constexpr (Lhs::S == POS && Rhs::S == NEG)
    return false;
  else if constexpr (Lhs::S == NEG && Rhs::S == POS)
    return false;

  // handle same signs -> underlying repr are both unsigned
  else if ((Lhs::S == POS && Rhs::S == POS) || (Lhs::S == NEG && Rhs::S == NEG)) {
    // TODO subtraction could lead to an unnecessary overflow
    return (rhs - lhs).repr == 0;
  }
}

constexpr bool
operator<(IsFixed auto lhs, IsFixed auto rhs)
{
  using Lhs = FixedTraits<decltype(lhs)>;
  using Rhs = FixedTraits<decltype(rhs)>;

  // handle nil
  if (Lhs::S == NIL && Rhs::S == NIL) // 1
    return false;
  if (Lhs::S == NIL && Rhs::S == NEG) // 2
    return false;
  if (Lhs::S == NIL && Rhs::S == POS) // 3
    return rhs.repr;
  if (Lhs::S == NIL && Rhs::S == VAR) // 4
    return 0 < rhs.repr;
  if (Lhs::S == NEG && Rhs::S == NIL) // 5
    return lhs.repr;
  if (Lhs::S == POS && Rhs::S == NIL) // 6
    return false;
  if (Lhs::S == VAR && Rhs::S == NIL) // 7
    return lhs.repr < 0;

  // handle contrary signs
  if (Lhs::S == POS && Rhs::S == NEG) // 8
    return false;
  if (Lhs::S == NEG && Rhs::S == POS) // 9
    return true;

  // handle same signs - todo avoid overflow
  if (Lhs::S == POS && Rhs::S == POS) // 10
    return (rhs - lhs).repr < 0;
  if (Lhs::S == NEG && Rhs::S == NEG) // 11
    return (lhs - rhs).repr < 0;

  // if (Lhs::S == VAR && Rhs::S == VAR) // 12
  //   return false;
  // if (Lhs::S == POS && Rhs::S == VAR) // 13
  //   return false;
  // if (Lhs::S == NEG && Rhs::S == VAR) // 14
  //   return false;
  // if (Lhs::S == VAR && Rhs::S == POS) // 15
  //   return false;
  // if (Lhs::S == VAR && Rhs::S == NEG) // 16
  //   return false;

  // handle first var by conversion
  if (Lhs::S == VAR) {
    if (lhs.repr < 0) {
      auto tmp = Fixed<NEG, Lhs::B, Lhs::P>{};
      tmp.repr = std::abs(lhs.repr);
      return tmp < rhs;
    }
    if (lhs.repr > 0) {
      auto tmp = Fixed<POS, Lhs::B, Lhs::P>{};
      tmp.repr = std::abs(lhs.repr);
      return tmp < rhs;
    }
    return (fixed_zero < rhs);
  }

  // handle second var by conversion
  if (Rhs::S == VAR) {
    if (rhs.repr < 0) {
      auto tmp = Fixed<NEG, Rhs::B, Rhs::P>{};
      tmp.repr = std::abs(rhs.repr);
      return lhs < tmp;
    }
    if (rhs.repr > 0) {
      auto tmp = Fixed<POS, Rhs::B, Rhs::P>{};
      tmp.repr = std::abs(rhs.repr);
      return lhs < tmp;
    }
    return lhs < fixed_zero;
  }
}
