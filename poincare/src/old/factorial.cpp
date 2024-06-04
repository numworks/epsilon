#include <poincare/layout.h>
#include <poincare/old/constant.h>
#include <poincare/old/factorial.h>
#include <poincare/old/parenthesis.h>
#include <poincare/old/rational.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/symbol.h>
#include <poincare/old/undefined.h>

#include <cmath>
#include <utility>

namespace Poincare {

// Property

bool FactorialNode::childAtIndexNeedsUserParentheses(const OExpression& child,
                                                     int childIndex) const {
  if (child.isNumber() &&
      static_cast<const Number&>(child).isPositive() == OMG::Troolean::False) {
    return true;
  }
  if (child.isOfType({Type::Conjugate, Type::Dependency})) {
    return childAtIndexNeedsUserParentheses(child.childAtIndex(0), childIndex);
  }
  return child.isOfType({Type::Subtraction, Type::Opposite,
                         Type::Multiplication, Type::Addition});
}

// Layout

bool FactorialNode::childNeedsSystemParenthesesAtSerialization(
    const PoolObject* child) const {
  /*  2
   * --- ! ---> [2/3]!
   *  3
   */
  return SerializationHelper::PostfixChildNeedsSystemParenthesesAtSerialization(
      child);
}

// Simplification

OExpression FactorialNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return Factorial(this).shallowReduce(reductionContext);
}

size_t FactorialNode::serialize(char* buffer, size_t bufferSize,
                                Preferences::PrintFloatMode floatDisplayMode,
                                int numberOfSignificantDigits) const {
  size_t numberOfChar = SerializationHelper::SerializeChild(
      childAtIndex(0), this, buffer, bufferSize, floatDisplayMode,
      numberOfSignificantDigits);
  if ((numberOfChar < 0) || (numberOfChar >= bufferSize - 1)) {
    return numberOfChar;
  }
  numberOfChar += SerializationHelper::CodePoint(
      buffer + numberOfChar, bufferSize - numberOfChar, '!');
  return numberOfChar;
}

OExpression Factorial::shallowReduce(ReductionContext reductionContext) {
  {
    OExpression e = SimplificationHelper::defaultShallowReduce(
        *this, &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::BanUnits,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  OExpression c = childAtIndex(0);
  if (c.otype() == ExpressionNode::Type::Rational) {
    Rational r = c.convert<Rational>();
    if (!r.isInteger() || r.isPositive() == OMG::Troolean::False) {
      return replaceWithUndefinedInPlace();
    }
    if (Integer(k_maxOperandValue).isLowerThan(r.unsignedIntegerNumerator())) {
      return *this;
    }
    Rational fact =
        Rational::Builder(Integer::Factorial(r.unsignedIntegerNumerator()));
    // Because fact < k_maxOperandValue!
    assert(!fact.numeratorOrDenominatorIsInfinity());
    replaceWithInPlace(fact);
    return std::move(fact);
  }
  if (c.isOfType({ExpressionNode::Type::ConstantMaths,
                  ExpressionNode::Type::ConstantPhysics})) {
    // e! = undef, i! = undef, pi! = undef
    return replaceWithUndefinedInPlace();
  }
  return *this;
}

#if 0
int Factorial::simplificationOrderGreaterType(const OExpression * e) const {
  if (SimplificationOrder(childAtIndex(0),e) == 0) {
    return 1;
  }
  return SimplificationOrder(childAtIndex(0), e);
}

int Factorial::simplificationOrderSameType(const OExpression * e) const {
  return SimplificationOrder(childAtIndex(0), e->childAtIndex(0));
}
#endif

}  // namespace Poincare
