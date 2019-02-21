#include <poincare/round.h>
#include <poincare/undefined.h>
#include <poincare/rational.h>
#include <poincare/power.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <assert.h>
#include <cmath>

namespace Poincare {

constexpr Expression::FunctionHelper Round::s_functionHelper;

int RoundNode::numberOfChildren() const { return Round::s_functionHelper.numberOfChildren(); }

Layout RoundNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(Round(this), floatDisplayMode, numberOfSignificantDigits, Round::s_functionHelper.name());
}

int RoundNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Round::s_functionHelper.name());
}

Expression RoundNode::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) {
  return Round(this).shallowReduce();
}

template<typename T>
Evaluation<T> RoundNode::templatedApproximate(Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> f1Input = childAtIndex(0)->approximate(T(), context, complexFormat, angleUnit);
  Evaluation<T> f2Input = childAtIndex(1)->approximate(T(), context, complexFormat, angleUnit);
  T f1 = f1Input.toScalar();
  T f2 = f2Input.toScalar();
  if (std::isnan(f2) || f2 != std::round(f2)) {
    return Complex<T>::Undefined();
  }
  T err = std::pow(10, std::floor(f2));
  return Complex<T>::Builder(std::round(f1*err)/err);
}

Round Round::Builder(Expression child0, Expression child1) {
  void * bufferNode = TreePool::sharedPool()->alloc(sizeof(RoundNode));
  RoundNode * node = new (bufferNode) RoundNode();
  TreeHandle h = TreeHandle::BuildWithBasicChildren(node, ArrayBuilder<Expression>(child0, child1).array(), 2);
  return static_cast<Round &>(h);
}

Expression Round::shallowReduce() {
  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }
#if MATRIX_EXACT_REDUCING
  if (childAtIndex(0).type() == ExpressionNode::Type::Matrix || childAtIndex(1).type() == ExpressionNode::Type::Matrix) {
    return Undefined::Builder();
  }
#endif
  /* We reduce only round(Rational, Rational). We do not reduce
   * round(Float, Float) which is equivalent to what is done in approximate. */
  if (childAtIndex(0).type() == ExpressionNode::Type::Rational && childAtIndex(1).type() == ExpressionNode::Type::Rational) {
    Rational r1 = childAtIndex(0).convert<Rational>();
    Rational r2 = childAtIndex(1).convert<Rational>();
    if (!r2.integerDenominator().isOne()) {
      Expression result = Undefined::Builder();
      replaceWithInPlace(result);
      return result;
    }
    const Rational ten = Rational::Builder(10);
    if (Power::RationalExponentShouldNotBeReduced(ten, r2)) {
      return *this;
    }
    Rational err = Rational::IntegerPower(ten, r2.signedIntegerNumerator());
    Rational mult = Rational::Multiplication(r1, err);
    IntegerDivision d = Integer::Division(mult.signedIntegerNumerator(), mult.integerDenominator());
    Integer rounding = d.quotient;
    Integer multDenominator = mult.integerDenominator();
    if (Rational::NaturalOrder(Rational::Builder(d.remainder, multDenominator), Rational::Builder(1,2)) >= 0) {
      rounding = Integer::Addition(rounding, Integer(1));
    }
    Rational result = Rational::Multiplication(Rational::Builder(rounding), Rational::IntegerPower(Rational::Builder(1,10), r2.signedIntegerNumerator()));
    if (result.numeratorOrDenominatorIsInfinity()) {
      return *this;
    }
    replaceWithInPlace(result);
    return result;
  }
  return *this;
}

}
