#include <poincare/division.h>
#include <poincare/fraction_layout.h>
#include <poincare/multiplication.h>
#include <poincare/opposite.h>
#include <poincare/power.h>
#include <poincare/rational.h>
#include <poincare/addition.h>
#include <poincare/subtraction.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <cmath>
#include <assert.h>
#include <string.h>
#include <float.h>

namespace Poincare {

TrinaryBoolean DivisionNode::isPositive(Context * context) const {
  TrinaryBoolean numeratorPositive = childAtIndex(0)->isPositive(context);
  TrinaryBoolean denominatorPositive = childAtIndex(1)->isPositive(context);
  if (numeratorPositive == TrinaryBoolean::Unknown || denominatorPositive == TrinaryBoolean::Unknown) {
    return TrinaryBoolean::Unknown;
  }
  return BinaryToTrinaryBool(numeratorPositive == denominatorPositive);
}

int DivisionNode::polynomialDegree(Context * context, const char * symbolName) const {
  if (childAtIndex(1)->polynomialDegree(context, symbolName) != 0) {
    return -1;
  }
  return childAtIndex(0)->polynomialDegree(context, symbolName);
}

Layout DivisionNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  const ExpressionNode * numerator = childAtIndex(0);
  const ExpressionNode * denominator = childAtIndex(1);
  return FractionLayout::Builder(numerator->createLayout(floatDisplayMode, numberOfSignificantDigits, context), denominator->createLayout(floatDisplayMode, numberOfSignificantDigits, context));
}

bool DivisionNode::childNeedsSystemParenthesesAtSerialization(const TreeNode * child) const {
  if (static_cast<const ExpressionNode *>(child)->isNumber() && Number(static_cast<const NumberNode *>(child)).isPositive() == TrinaryBoolean::False) {
    return true;
  }
  if (static_cast<const ExpressionNode *>(child)->type() == Type::Rational && !static_cast<const RationalNode *>(child)->isInteger()) {
    return true;
  }
  return static_cast<const ExpressionNode *>(child)->isOfType({Type::Subtraction, Type::Opposite, Type::Multiplication, Type::Division, Type::Addition, Type::PercentSimple, Type::PercentAddition, Type::MixedFraction});
}

int DivisionNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Infix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "/");
}

Expression DivisionNode::shallowReduce(const ReductionContext& reductionContext) {
  return Division(this).shallowReduce(reductionContext);
}

template<typename T> Complex<T> DivisionNode::computeOnComplex(const std::complex<T> c, const std::complex<T> d, Preferences::ComplexFormat complexFormat) {
  constexpr T zero = static_cast<T>(0.0);
  if (d.real() == zero && d.imag() == zero) {
    return Complex<T>::Undefined();
  }
  // Special case to prevent (inf,0)/(1,0) from returning (inf, nan).
  if (std::isinf(std::abs(c)) || std::isinf(std::abs(d))) {
    // Handle case of pure imaginary/real divisions
    if (c.imag() == zero && d.imag() == zero) {
      return Complex<T>::Builder(c.real()/d.real(), zero);
    }
    if (c.real() == zero && d.real() == zero) {
      return Complex<T>::Builder(c.imag()/d.imag(), zero);
    }
    if (c.imag() == zero && d.real() == zero) {
      return Complex<T>::Builder(zero, -c.real()/d.imag());
    }
    if (c.real() == zero && d.imag() == zero) {
      return Complex<T>::Builder(zero, c.imag()/d.real());
    }
    // Other cases are left to the standard library, and might return NaN.
  }
  return Complex<T>::Builder(c/d);
}

// Division
Expression Division::shallowReduce(ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(*this, &reductionContext, SimplificationHelper::BooleanReduction::UndefinedOnBooleans);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  /* For matrices: we decided that A/B is undefined, as it is a non-standard
   * shortcut */
  if (childAtIndex(1).deepIsMatrix(reductionContext.context(), reductionContext.shouldCheckMatrices())) {
    return replaceWithUndefinedInPlace();
  }
  Expression p = Power::Builder(childAtIndex(1), Rational::Builder(-1));
  Multiplication m = Multiplication::Builder(childAtIndex(0), p);
  p.shallowReduce(reductionContext); // For instance: Division::Builder(2,1). p would be 1^(-1) which can be simplified
  replaceWithInPlace(m);
  return m.shallowReduce(reductionContext);
}

}
