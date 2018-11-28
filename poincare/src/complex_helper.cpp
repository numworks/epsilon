#include <poincare/complex_helper.h>
#include <poincare/cosine.h>
#include <poincare/sine.h>
#include <poincare/multiplication.h>
#include <poincare/rational.h>

namespace Poincare {

Expression ComplexHelper::cartesianPartOfComplexFunction(const ExpressionNode * node, Context & context, Preferences::AngleUnit angleUnit, bool real) {
  Expression e(node);
  Expression opImaginaryPart = e.childAtIndex(0).imaginaryPart(context, angleUnit);
  if (opImaginaryPart.isUninitialized() || opImaginaryPart.type() != ExpressionNode::Type::Rational || !static_cast<Rational &>(opImaginaryPart).isZero()) {
    return Expression();
  }
  if (real) {
    return e.clone().deepReduce(context, angleUnit, ExpressionNode::ReductionTarget::BottomUpComputation);
  }
  return Rational(0);
}

Expression ComplexHelper::imaginaryPartRealFunction(const ExpressionNode * e, Context & context, Preferences::AngleUnit angleUnit) {
  return Rational(0);
}

Expression ComplexHelper::complexCartesianPartFromPolarParts(const ExpressionNode * node, Context & context, Preferences::AngleUnit angleUnit, bool isReal) {
  Expression e(node);
  Expression r = e.complexNorm(context, angleUnit);
  Expression th = e.complexArgument(context, angleUnit);
  if (r.isUninitialized() || th.isUninitialized()) {
    return Expression();
  }
  Expression argument = angleUnit == Preferences::AngleUnit::Radian ? th : th.radianToDegree(context, angleUnit, ExpressionNode::ReductionTarget::BottomUpComputation);
  Expression trigo;
  if (isReal) {
    trigo = Cosine::Builder(argument);
  } else {
    trigo = Sine::Builder(argument);
  }
  return Multiplication(r, trigo.shallowReduce(context, angleUnit, ExpressionNode::ReductionTarget::BottomUpComputation)).shallowReduce(context, angleUnit, ExpressionNode::ReductionTarget::BottomUpComputation);
}

}
