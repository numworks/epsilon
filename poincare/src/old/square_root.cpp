#include <assert.h>
#include <ion.h>
#include <poincare/old/addition.h>
#include <poincare/old/division.h>
#include <poincare/old/layout.h>
#include <poincare/old/opposite.h>
#include <poincare/old/power.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/sign_function.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/square_root.h>
#include <poincare/old/subtraction.h>
#include <poincare/old/undefined.h>

#include <cmath>

namespace Poincare {

int SquareRootNode::numberOfChildren() const {
  return SquareRoot::s_functionHelper.numberOfChildren();
}

size_t SquareRootNode::serialize(char *buffer, size_t bufferSize,
                                 Preferences::PrintFloatMode floatDisplayMode,
                                 int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      SquareRoot::s_functionHelper.aliasesList().mainAlias());
}

template <typename T>
std::complex<T> SquareRootNode::computeOnComplex(
    const std::complex<T> c, Preferences::ComplexFormat,
    Preferences::AngleUnit angleUnit) {
  std::complex<T> result = std::sqrt(c);
  return ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable(result,
                                                                      c);
}

OExpression SquareRootNode::shallowReduce(
    const ReductionContext &reductionContext) {
  return SquareRoot(this).shallowReduce(reductionContext);
}

bool SquareRoot::SplitRadical(OExpression term, OExpression *factor,
                              OExpression *underRoot) {
  /* We expect term to be a reduction of a√b, with a or b eventually equal to 1
   */
  if (term.otype() == ExpressionNode::Type::Rational) {
    *factor = term;
    *underRoot = Rational::Builder(1);
    return true;
  }
  if (term.otype() == ExpressionNode::Type::Power) {
    return SplitRadical(Multiplication::Builder(Rational::Builder(1), term),
                        factor, underRoot);
  }
  if (term.otype() == ExpressionNode::Type::Multiplication &&
      term.numberOfChildren() == 2) {
    OExpression factor1 = term.childAtIndex(0), factor2 = term.childAtIndex(1);
    if (factor1.otype() == ExpressionNode::Type::Rational &&
        (factor2.otype() == ExpressionNode::Type::Power &&
         factor2.childAtIndex(1).otype() == ExpressionNode::Type::Rational &&
         factor2.childAtIndex(0).otype() == ExpressionNode::Type::Rational)) {
      OExpression exponent = factor2.childAtIndex(1);
      if (static_cast<Rational &>(exponent).isHalf()) {
        *factor = factor1;
        *underRoot = factor2.childAtIndex(0);
        return true;
      }
    }
  }
  return false;
}

OExpression SquareRoot::ReduceNestedRadicals(
    OExpression a, OExpression b, OExpression c, OExpression d,
    const ReductionContext &reductionContext) {
  assert(a.otype() == ExpressionNode::Type::Rational &&
         b.otype() == ExpressionNode::Type::Rational &&
         c.otype() == ExpressionNode::Type::Rational &&
         d.otype() == ExpressionNode::Type::Rational);
  /* We want to go from √(a√b + c√d) to root(w,4)×√x×√(y+√z), because √(y+√z)
   * is very easy to denest. */
  Rational rA = static_cast<Rational &>(a), rB = static_cast<Rational &>(b),
           rC = static_cast<Rational &>(c), rD = static_cast<Rational &>(d);
  Rational rA2B =
      Rational::Multiplication(Rational::IntegerPower(rA, Integer(2)), rB);
  Rational rC2D =
      Rational::Multiplication(Rational::IntegerPower(rC, Integer(2)), rD);
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
  if (y2.numeratorOrDenominatorIsInfinity() ||
      w.numeratorOrDenominatorIsInfinity() ||
      x.numeratorOrDenominatorIsInfinity() ||
      z.numeratorOrDenominatorIsInfinity()) {
    return OExpression();
  }
  /* √(y+√z) can be turned into √u+√v if √(y^2-z) is rational. Because of our
   * choice of w, x, y and z, we know that y^2 > z. */
  Rational y2MinusZ = Rational::Addition(
      y2, Rational::Multiplication(z, Rational::Builder(-1)));
  if (y2MinusZ.numeratorOrDenominatorIsInfinity()) {
    return OExpression();
  }
  OExpression delta = Power::Builder(y2MinusZ, Rational::Builder(1, 2))
                          .shallowReduce(reductionContext);
  if (delta.otype() != ExpressionNode::Type::Rational) {
    return OExpression();
  }
  Rational rDelta = static_cast<Rational &>(delta);
  OExpression left =
      Power::Builder(Rational::Multiplication(Rational::Addition(y, rDelta),
                                              Rational::Builder(1, 2)),
                     Rational::Builder(1, 2));
  OExpression right = Power::Builder(
      Rational::Multiplication(
          Rational::Addition(
              y, Rational::Multiplication(rDelta, Rational::Builder(-1))),
          Rational::Builder(1, 2)),
      Rational::Builder(1, 2));
  OExpression result = Multiplication::Builder(
      {Power::Builder(w, Rational::Builder(1, 4)),
       Power::Builder(x, Rational::Builder(1, 2)),
       Addition::Builder(left, subtract ? Opposite::Builder(right) : right)});
  return result.deepReduce(reductionContext);
}

OExpression SquareRoot::shallowReduce(ReductionContext reductionContext) {
  {
    OExpression e = SimplificationHelper::defaultShallowReduce(
        *this, &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::KeepUnits,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  OExpression c = childAtIndex(0);
  Power p = Power::Builder(c, Rational::Builder(1, 2));
  replaceWithInPlace(p);
  return p.shallowReduce(reductionContext);
}

}  // namespace Poincare
