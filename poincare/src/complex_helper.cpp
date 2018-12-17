#include <poincare/complex_helper.h>
#include <poincare/addition.h>
#include <poincare/arc_tangent.h>
#include <poincare/cosine.h>
#include <poincare/constant.h>
#include <poincare/division.h>
#include <poincare/multiplication.h>
#include <poincare/rational.h>
#include <poincare/square_root.h>
#include <poincare/sine.h>
#include <poincare/sign_function.h>
#include <poincare/subtraction.h>
#include <poincare/power.h>

namespace Poincare {

ComplexCartesian ComplexHelper::complexCartesianComplexFunction(const ExpressionNode * node, Context & context, Preferences::AngleUnit angleUnit) {
  Expression e(node);
  ComplexCartesian complexChild = e.childAtIndex(0).complexCartesian(context, angleUnit);
  if (complexChild.isUninitialized()) {
    return ComplexCartesian();
  }
  Expression childImaginaryPart = complexChild.imag();
  assert(!childImaginaryPart.isUninitialized());
  if (childImaginaryPart.type() != ExpressionNode::Type::Rational || !static_cast<Rational &>(childImaginaryPart).isZero()) {
    return ComplexCartesian();
  }
  return ComplexCartesian::Builder(
      e.clone(),
      Rational(0)
    );
}

ComplexCartesian ComplexHelper::complexCartesianRealFunction(const ExpressionNode * e, Context & context, Preferences::AngleUnit angleUnit) {
  return ComplexCartesian::Builder(
    Expression(e).clone(),
    Rational(0)
  );
}

Expression ComplexHelper::complexCartesianFromComplexPolarHelper(Expression norm, Expression trigo, Context & context, Preferences::AngleUnit angleUnit) {
  return Multiplication(norm, trigo.shallowReduce(context, angleUnit, ExpressionNode::ReductionTarget::BottomUpComputation)).shallowReduce(context, angleUnit, ExpressionNode::ReductionTarget::BottomUpComputation);
}

ComplexCartesian ComplexHelper::complexCartesianFromComplexPolar(const ExpressionNode * node, Context & context, Preferences::AngleUnit angleUnit) {
  Expression e(node);
  ComplexPolar polar = e.complexPolar(context, angleUnit);
  if (polar.isUninitialized()) {
    return ComplexCartesian();
  }
  Expression r = polar.norm();
  Expression th = polar.arg();
  assert(!r.isUninitialized() && !th.isUninitialized());
  Expression argument = th;
  return ComplexCartesian::Builder(
      complexCartesianFromComplexPolarHelper(r.clone(), Cosine::Builder(argument.clone()), context, angleUnit),
      complexCartesianFromComplexPolarHelper(r, Sine::Builder(argument), context, angleUnit)
    );
}

ComplexPolar ComplexHelper::complexPolarFromComplexCartesian(const ExpressionNode * node, Context & context, Preferences::AngleUnit angleUnit) {
  Expression e(node);
  ComplexCartesian cartesian = e.complexCartesian(context, angleUnit);
  if (cartesian.isUninitialized()) {
    return ComplexPolar();
  }
  Expression a = cartesian.real();
  Expression b = cartesian.imag();
  assert(!a.isUninitialized() && !b.isUninitialized());
  // Step 1: find norm
  Expression norm;
  ExpressionNode::Sign s = e.sign(&context, angleUnit);
  if (s == ExpressionNode::Sign::Positive) {
    // Case 1: the expression is positive real
    norm = e.clone();
  } else if (s == ExpressionNode::Sign::Negative) {
    // Case 2: the argument is negative real
    norm = e.clone().setSign(ExpressionNode::Sign::Positive, &context, angleUnit);
  } else {
    // Case 3: the argument is complex or of unknown approximation
    // sqrt(a^2+b^2)
    norm = SquareRoot::Builder(
        Addition(
          Power(a.clone(), Rational(2)).shallowReduce(context, angleUnit, ExpressionNode::ReductionTarget::BottomUpComputation),
          Power(b.clone(), Rational(2)).shallowReduce(context, angleUnit, ExpressionNode::ReductionTarget::BottomUpComputation)
        ).shallowReduce(context, angleUnit, ExpressionNode::ReductionTarget::BottomUpComputation)
      ).shallowReduce(context, angleUnit, ExpressionNode::ReductionTarget::BottomUpComputation);
  }
  // Step 2: find the argument
  Expression argument;
  float bApprox = b.approximateToScalar<float>(context, angleUnit);
  if (bApprox != 0.0f) {
    // if b != 0, argument = sign(b) * Pi/2 - arctan(a/b)
    // First, compute arctan(a/b) or (Pi/180)*arctan(a/b)
    Expression arcTangent = ArcTangent::Builder(Division(a, b.clone()).shallowReduce(context, angleUnit, ExpressionNode::ReductionTarget::BottomUpComputation)).shallowReduce(context, angleUnit, ExpressionNode::ReductionTarget::BottomUpComputation);
    // Then, compute sign(b) * Pi/2 - arctan(a/b)
    argument = Subtraction(
        Multiplication(
          SignFunction::Builder(b).shallowReduce(context, angleUnit, ExpressionNode::ReductionTarget::BottomUpComputation),
          Division(Constant(Ion::Charset::SmallPi), Rational(2)).shallowReduce(context, angleUnit, ExpressionNode::ReductionTarget::BottomUpComputation)
        ).shallowReduce(context, angleUnit, ExpressionNode::ReductionTarget::BottomUpComputation),
        arcTangent
      ).shallowReduce(context, angleUnit, ExpressionNode::ReductionTarget::BottomUpComputation);
  } else {
    // if b == 0, argument = (1-sign(a))*Pi/2
    argument = Multiplication(
        Subtraction(
          Rational(1),
          SignFunction::Builder(a).shallowReduce(context, angleUnit, ExpressionNode::ReductionTarget::BottomUpComputation)
        ).shallowReduce(context, angleUnit, ExpressionNode::ReductionTarget::BottomUpComputation),
        Division(Constant(Ion::Charset::SmallPi), Rational(2)).shallowReduce(context, angleUnit, ExpressionNode::ReductionTarget::BottomUpComputation)
      ).shallowReduce(context, angleUnit, ExpressionNode::ReductionTarget::BottomUpComputation);
  }
  return ComplexPolar::Builder(norm, argument);

}

Expression ComplexHelper::complexSquareNormComplexCartesian(Expression real, Expression imag, Context & context, Preferences::AngleUnit angleUnit) {
  assert(!real.isUninitialized() && !imag.isUninitialized());
  return Addition(
    Power(
      real,
      Rational(2)
    ).shallowReduce(context, angleUnit, ExpressionNode::ReductionTarget::BottomUpComputation),
    Power(
      imag,
      Rational(2)
    ).shallowReduce(context, angleUnit, ExpressionNode::ReductionTarget::BottomUpComputation)
  ).shallowReduce(context, angleUnit, ExpressionNode::ReductionTarget::BottomUpComputation);
}

}
