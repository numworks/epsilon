#include <poincare/square_root.h>
#include <poincare/addition.h>
#include <poincare/division.h>
#include <poincare/layout_helper.h>
#include <poincare/nth_root_layout.h>
#include <poincare/opposite.h>
#include <poincare/power.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/sign_function.h>
#include <poincare/subtraction.h>
#include <poincare/undefined.h>

#include <assert.h>
#include <cmath>
#include <ion.h>

namespace Poincare {

int SquareRootNode::numberOfChildren() const { return SquareRoot::s_functionHelper.numberOfChildren(); }

Layout SquareRootNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  return NthRootLayout::Builder(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits, context));
}

int SquareRootNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, SquareRoot::s_functionHelper.aliasesList().mainAlias());
}

template<typename T>
Complex<T> SquareRootNode::computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit) {
  std::complex<T> result = std::sqrt(c);
  return Complex<T>::Builder(ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable(result, std::complex<T>(std::log(std::abs(c)), std::arg(c))));
}

Expression SquareRootNode::shallowReduce(const ReductionContext& reductionContext) {
  return SquareRoot(this).shallowReduce(reductionContext);
}

bool SquareRoot::SplitRadical(Expression term, Expression * factor, Expression * underRoot) {
  /* We expect term to be a reduction of a√b, with a or b eventually equal to 1 */
  if (term.type() == ExpressionNode::Type::Rational) {
    *factor = term;
    *underRoot = Rational::Builder(1);
    return true;
  }
  if (term.type() == ExpressionNode::Type::Power) {
    return SplitRadical(Multiplication::Builder(Rational::Builder(1), term), factor, underRoot);
  }
  if (term.type() == ExpressionNode::Type::Multiplication && term.numberOfChildren() == 2) {
    Expression factor1 = term.childAtIndex(0), factor2 = term.childAtIndex(1);
    if (factor1.type() == ExpressionNode::Type::Rational
     && (factor2.type() == ExpressionNode::Type::Power && factor2.childAtIndex(1).type() == ExpressionNode::Type::Rational && factor2.childAtIndex(0).type() == ExpressionNode::Type::Rational)) {
      Expression exponent = factor2.childAtIndex(1);
      if (static_cast<Rational &>(exponent).isHalf()) {
        *factor = factor1;
        *underRoot = factor2.childAtIndex(0);
        return true;
      }
    }
  }
  return false;
}

Expression SquareRoot::ReduceNestedRadicals(Expression a, Expression b, Expression c, Expression d, const ExpressionNode::ReductionContext& reductionContext) {
  assert(a.type() == ExpressionNode::Type::Rational && b.type() == ExpressionNode::Type::Rational && c.type() == ExpressionNode::Type::Rational && d.type() == ExpressionNode::Type::Rational);
  Expression result;
  /* We want to go from √(a√b + c√d) to root(w,4)×√x×√(y+√z), because √(y+√z)
   * is very easy to denest. */
  Rational rA = static_cast<Rational &>(a), rB = static_cast<Rational &>(b), rC = static_cast<Rational &>(c), rD = static_cast<Rational &>(d);
  Rational rA2B = Rational::Multiplication(Rational::IntegerPower(rA, Integer(2)), rB);
  Rational rC2D = Rational::Multiplication(Rational::IntegerPower(rC, Integer(2)), rD);
  Rational w(0), x(0), y(0), z(0);
  if (Rational::NaturalOrder(rA2B, rC2D) > 0) {
    w = rB;
    x = rC;
    y = rA;
    z = rD;
  } else {
    w = rD;
    x = rA;
    y = rC;
    z = rB;
  }
  bool subtract = x.isNegative() != y.isNegative();
  if (subtract) {
    x = Rational::Multiplication(x, Rational::Builder(-1));
  }
  z = Rational::Multiplication(z, Rational::IntegerPower(w, Integer(-1)));
  y = Rational::Multiplication(y, Rational::IntegerPower(x, Integer(-1)));
  Rational y2 = Rational::IntegerPower(y, Integer(2));
  if (y2.numeratorOrDenominatorIsInfinity() || w.numeratorOrDenominatorIsInfinity() || x.numeratorOrDenominatorIsInfinity() || z.numeratorOrDenominatorIsInfinity()) {
    return result;
  }
  /* √(y+√z) can be turned into √u+√v if √(y^2-z) is rational. Because of our
   * choice of w, x, y and z, we know that y^2 > z. */
  Expression delta = Power::Builder(Rational::Addition(y2, Rational::Multiplication(z, Rational::Builder(-1))), Rational::Builder(1, 2)).shallowReduce(reductionContext);
  if (delta.type() != ExpressionNode::Type::Rational) {
    return result;
  }
  Rational rDelta = static_cast<Rational &>(delta);
  Expression left = Power::Builder(Rational::Multiplication(Rational::Addition(y, rDelta), Rational::Builder(1, 2)), Rational::Builder(1, 2));
  Expression right = Power::Builder(Rational::Multiplication(Rational::Addition(y, Rational::Multiplication(rDelta, Rational::Builder(-1))), Rational::Builder(1, 2)), Rational::Builder(1, 2));
  result = Multiplication::Builder({
      Power::Builder(w, Rational::Builder(1, 4)),
      Power::Builder(x, Rational::Builder(1, 2)),
      Addition::Builder(left, subtract ? Opposite::Builder(right) : right)});
  return result.deepReduce(reductionContext);
}

Expression SquareRoot::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(
        *this,
        &reductionContext,
        SimplificationHelper::UnitReduction::KeepUnits,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists
        );
    if (!e.isUninitialized()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
  Power p = Power::Builder(c, Rational::Builder(1, 2));
  replaceWithInPlace(p);
  return p.shallowReduce(reductionContext);
}

}
