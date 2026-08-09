#pragma once

#include <utility.hpp> // SpecializationOf

#include <concepts>    // std::same_as
#include <type_traits> // std::remove_cvref_t
#include <utility>     // std::move

namespace rational {

namespace ast {

struct NodeTag
{};

template<typename T>
concept Node = std::same_as<typename T::tag, NodeTag> && requires(T const node) { node.evaluate(); };

struct OneNode
{
  using tag = NodeTag;

  [[nodiscard]] constexpr auto evaluate() const -> OneNode const&
  {
    return *this;
  }
};

template<typename Value>
struct RefValueNode
{
  using tag = NodeTag;

  Value const* mValue;

  constexpr explicit RefValueNode(Value const* value)
    : mValue(value)
  {
  }
  constexpr ~RefValueNode() = default;

  constexpr RefValueNode(RefValueNode&&)      = default;
  constexpr RefValueNode(RefValueNode const&) = default;

  constexpr auto operator=(RefValueNode&&) -> RefValueNode&      = default;
  constexpr auto operator=(RefValueNode const&) -> RefValueNode& = default;

  [[nodiscard]] constexpr auto evaluate() const -> Value const&
  {
    return *mValue;
  }
};

template<typename Value>
struct ValueNode
{
  using tag = NodeTag;

  Value mValue;

  constexpr explicit ValueNode(Value value)
    : mValue(std::move(value))
  {
  }
  constexpr ~ValueNode() = default;

  constexpr ValueNode(ValueNode&&)      = default;
  constexpr ValueNode(ValueNode const&) = default;

  constexpr auto operator=(ValueNode&&) -> ValueNode&      = default;
  constexpr auto operator=(ValueNode const&) -> ValueNode& = default;

  [[nodiscard]] constexpr auto evaluate() const -> Value const&
  {
    return mValue;
  }
};

template<Node Lhs, Node Rhs>
struct AdditionNode
{
  using tag = NodeTag;

  Lhs mLhs;
  Rhs mRhs;

  constexpr AdditionNode(Lhs const& lhs, Rhs const& rhs)
    : mLhs(lhs)
    , mRhs(rhs)
  {
  }
  constexpr ~AdditionNode() = default;

  constexpr AdditionNode(AdditionNode&&)      = default;
  constexpr AdditionNode(AdditionNode const&) = default;

  constexpr auto operator=(AdditionNode&&) -> AdditionNode&      = default;
  constexpr auto operator=(AdditionNode const&) -> AdditionNode& = default;

  [[nodiscard]] constexpr auto evaluate() const
  {
    return mLhs.evaluate() + mRhs.evaluate();
  }
};

template<Node Lhs, Node Rhs>
struct SubtractionNode
{
  using tag = NodeTag;

  Lhs mLhs;
  Rhs mRhs;

  constexpr SubtractionNode(Lhs const& lhs, Rhs const& rhs)
    : mLhs(lhs)
    , mRhs(rhs)
  {
  }
  constexpr ~SubtractionNode() = default;

  constexpr SubtractionNode(SubtractionNode&&)      = default;
  constexpr SubtractionNode(SubtractionNode const&) = default;

  constexpr auto operator=(SubtractionNode&&) -> SubtractionNode&      = default;
  constexpr auto operator=(SubtractionNode const&) -> SubtractionNode& = default;

  [[nodiscard]] constexpr auto evaluate() const
  {
    return mLhs.evaluate() - mRhs.evaluate();
  }
};

template<Node Lhs, Node Rhs>
struct MultiplicationNode
{
  using tag = NodeTag;

  Lhs mLhs;
  Rhs mRhs;

  constexpr MultiplicationNode(Lhs const& lhs, Rhs const& rhs)
    : mLhs(lhs)
    , mRhs(rhs)
  {
  }
  constexpr ~MultiplicationNode() = default;

  constexpr MultiplicationNode(MultiplicationNode&&)      = default;
  constexpr MultiplicationNode(MultiplicationNode const&) = default;

  constexpr auto operator=(MultiplicationNode&&) -> MultiplicationNode&      = default;
  constexpr auto operator=(MultiplicationNode const&) -> MultiplicationNode& = default;

  [[nodiscard]] constexpr auto evaluate() const
  {
    if constexpr (std::same_as<OneNode, Lhs>) {
      return mRhs.evaluate();
    } else if constexpr (std::same_as<OneNode, Rhs>) {
      return mLhs.evaluate();
    } else {
      return mLhs.evaluate() * mRhs.evaluate();
    }
  }
};

template<Node NumeratorNode, Node DenominatorNode = OneNode>
struct RationalExpression
{
  NumeratorNode   mNumerator;
  DenominatorNode mDenominator;

  constexpr RationalExpression(NumeratorNode const& numerator, DenominatorNode const& denominator)
    : mNumerator(numerator)
    , mDenominator(denominator)
  {
  }
  // NOLINTBEGIN(*-explicit-conversions)
  constexpr RationalExpression(NumeratorNode const& numerator)
    : mNumerator(numerator)
    , mDenominator(OneNode{})
  {
  }
  // NOLINTEND(*-explicit-conversions)
  constexpr ~RationalExpression() = default;

  constexpr RationalExpression(RationalExpression&&)      = default;
  constexpr RationalExpression(RationalExpression const&) = default;

  constexpr auto operator=(RationalExpression&&) -> RationalExpression&      = default;
  constexpr auto operator=(RationalExpression const&) -> RationalExpression& = default;
};

template<typename T>
concept Expression = Node<T> || SpecializationOf<T, RationalExpression>;

constexpr auto
operator+(Node auto const& lhs, Node auto const& rhs)
{
  static_assert(!std::same_as<OneNode, std::remove_cvref_t<decltype(lhs)>>);
  static_assert(!std::same_as<OneNode, std::remove_cvref_t<decltype(rhs)>>);
  return AdditionNode{ lhs, rhs };
}

constexpr auto
operator-(Node auto const& lhs, Node auto const& rhs)
{
  static_assert(!std::same_as<OneNode, std::remove_cvref_t<decltype(lhs)>>);
  static_assert(!std::same_as<OneNode, std::remove_cvref_t<decltype(rhs)>>);
  return SubtractionNode{ lhs, rhs };
}

constexpr auto
operator*(Node auto const& lhs, Node auto const& rhs)
{
  if constexpr (std::same_as<OneNode, decltype(lhs)>) {
    return rhs;
  } else if constexpr (std::same_as<OneNode, decltype(rhs)>) {
    return lhs;
  } else {
    return MultiplicationNode{ lhs, rhs };
  }
}

constexpr auto
operator/(Node auto const& lhs, Node auto const& rhs)
{
  static_assert(!std::same_as<OneNode, std::remove_cvref_t<decltype(lhs)>>);
  return RationalExpression{ lhs, rhs };
}

constexpr auto
operator*(Expression auto const& lhs, Expression auto const& rhs)
{
  auto const rationalLhs = RationalExpression{ lhs };
  auto const rationalRhs = RationalExpression{ rhs };
  return RationalExpression{
    rationalLhs.mNumerator * rationalRhs.mNumerator,
    rationalLhs.mDenominator * rationalRhs.mDenominator,
  };
}

constexpr auto
operator/(Expression auto const& lhs, Expression auto const& rhs)
{
  auto const rationalLhs = RationalExpression{ lhs };
  auto const rationalRhs = RationalExpression{ rhs };
  return RationalExpression{
    rationalLhs.mNumerator * rationalRhs.mDenominator,
    rationalLhs.mDenominator * rationalRhs.mNumerator,
  };
}

constexpr auto
operator+(Expression auto const& lhs, Expression auto const& rhs)
{
  auto const rationalLhs = RationalExpression{ lhs };
  auto const rationalRhs = RationalExpression{ rhs };
  return RationalExpression{
    rationalLhs.mNumerator * rationalRhs.mDenominator + rationalRhs.mNumerator * rationalLhs.mDenominator,
    rationalLhs.mDenominator * rationalRhs.mDenominator,
  };
}

constexpr auto
operator-(Expression auto const& lhs, Expression auto const& rhs)
{
  auto const rationalLhs = RationalExpression{ lhs };
  auto const rationalRhs = RationalExpression{ rhs };
  return RationalExpression{
    rationalLhs.mNumerator * rationalRhs.mDenominator - rationalRhs.mNumerator * rationalLhs.mDenominator,
    rationalLhs.mDenominator * rationalRhs.mDenominator,
  };
}

[[nodiscard]] constexpr bool
isNegative(Node auto const& node)
{
  if constexpr (std::same_as<OneNode, std::remove_cvref_t<decltype(node)>>) {
    return false;
  } else {
    auto const value = node.evaluate();
    return value < value - value;
  }
}

constexpr auto
applyCompare(auto const& compare, Expression auto const& lhs, Expression auto const& rhs)
{
  auto const rationalLhs = RationalExpression{ lhs };
  auto const rationalRhs = RationalExpression{ rhs };

  auto comparisonLhs = rationalLhs.mNumerator * rationalRhs.mDenominator;
  auto comparisonRhs = rationalRhs.mNumerator * rationalLhs.mDenominator;

  if (isNegative(rationalLhs.mDenominator) != isNegative(rationalRhs.mDenominator)) {
    return compare(comparisonRhs.evaluate(), comparisonLhs.evaluate());
  }

  return compare(comparisonLhs.evaluate(), comparisonRhs.evaluate());
}

} // namespace ast

template<typename Value>
constexpr auto
val(Value const* value)
{
  return ast::RefValueNode<Value>{ value };
}

template<typename Value>
constexpr auto
val(Value value)
{
  return ast::ValueNode<Value>{ std::move(value) };
}

// NOLINTBEGIN(*-unused-using-decls)
using ast::operator+;
using ast::operator-;
using ast::operator*;
using ast::operator/;
// NOLINTEND(*-unused-using-decls)

constexpr bool
operator==(auto const& lhs, auto const& rhs)
{
  constexpr auto compare = [](auto const& lhsEvaluated, auto const& rhsEvaluated) { return lhsEvaluated == rhsEvaluated; };
  return ast::applyCompare(compare, lhs, rhs);
}

constexpr bool
operator!=(auto const& lhs, auto const& rhs)
{
  constexpr auto compare = [](auto const& lhsEvaluated, auto const& rhsEvaluated) { return lhsEvaluated != rhsEvaluated; };
  return ast::applyCompare(compare, lhs, rhs);
}

constexpr bool
operator<(auto const& lhs, auto const& rhs)
{
  constexpr auto compare = [](auto const& lhsEvaluated, auto const& rhsEvaluated) { return lhsEvaluated < rhsEvaluated; };
  return ast::applyCompare(compare, lhs, rhs);
}

constexpr bool
operator>(auto const& lhs, auto const& rhs)
{
  constexpr auto compare = [](auto const& lhsEvaluated, auto const& rhsEvaluated) { return lhsEvaluated > rhsEvaluated; };
  return ast::applyCompare(compare, lhs, rhs);
}

constexpr bool
operator<=(auto const& lhs, auto const& rhs)
{
  constexpr auto compare = [](auto const& lhsEvaluated, auto const& rhsEvaluated) { return lhsEvaluated <= rhsEvaluated; };
  return ast::applyCompare(compare, lhs, rhs);
}

constexpr bool
operator>=(auto const& lhs, auto const& rhs)
{
  constexpr auto compare = [](auto const& lhsEvaluated, auto const& rhsEvaluated) { return lhsEvaluated >= rhsEvaluated; };
  return ast::applyCompare(compare, lhs, rhs);
}

constexpr auto
operator<=>(auto const& lhs, auto const& rhs)
{
  constexpr auto compare = [](auto const& lhsEvaluated, auto const& rhsEvaluated) { return lhsEvaluated <=> rhsEvaluated; };
  return ast::applyCompare(compare, lhs, rhs);
}
} // namespace rational
