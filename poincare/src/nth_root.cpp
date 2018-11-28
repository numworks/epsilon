#include <poincare/nth_root.h>
#include <poincare/addition.h>
#include <poincare/constant.h>
#include <poincare/division.h>
#include <poincare/naperian_logarithm.h>
#include <poincare/power.h>
#include <poincare/undefined.h>
#include <poincare/nth_root_layout.h>
#include <poincare/subtraction.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <assert.h>
#include <cmath>

namespace Poincare {

constexpr Expression::FunctionHelper NthRoot::s_functionHelper;

int NthRootNode::numberOfChildren() const { return NthRoot::s_functionHelper.numberOfChildren(); }

Expression NthRootNode::complexPolarPart(Context & context, Preferences::AngleUnit angleUnit, bool isNorm) const {
  NthRoot e(this);
  // NthRoot(r*e^(i*th), c+id)
  Expression r = e.childAtIndex(0).complexNorm(context, angleUnit);
  Expression th = e.childAtIndex(0).complexArgument(context, angleUnit);
  Expression c = e.childAtIndex(1).realPart(context, angleUnit);
  Expression d = e.childAtIndex(1).imaginaryPart(context, angleUnit);
  if (r.isUninitialized() || th.isUninitialized() || c.isUninitialized() || d.isUninitialized()) {
    return Expression();
  }
  Expression denominator = 
    Addition(
      Power(c.clone(), Rational(2)).shallowReduce(context, angleUnit, ReductionTarget::BottomUpComputation),
      Power(d.clone(), Rational(2)).shallowReduce(context, angleUnit, ReductionTarget::BottomUpComputation)
    ).shallowReduce(context, angleUnit, ReductionTarget::BottomUpComputation);
  if (isNorm) {
    // R = e^((c*ln(r)+th*d)/(c^2+d^2))
    // R = r^(c/(c^2+d^2))*e^(th*d/(c^2+d^2))
    return Multiplication(
            Power(
              r,
              Division(c, denominator.clone()).shallowReduce(context, angleUnit, ReductionTarget::BottomUpComputation)
            ).shallowReduce(context, angleUnit, ReductionTarget::BottomUpComputation),
            Power(
              Constant(Ion::Charset::Exponential),
              Division(
                Multiplication(d, th).shallowReduce(context, angleUnit, ReductionTarget::BottomUpComputation),
                denominator)
              ).shallowReduce(context, angleUnit, ReductionTarget::BottomUpComputation)
            ).shallowReduce(context, angleUnit, ReductionTarget::BottomUpComputation);
    //return Power(Constant(Ion::Charset::Exponential), Division(Addition(Multiplication(c, NaperianLogarithm::Builder(r)), Multiplication(d, th)), denominator));
  } else {
    // TH = (th*c-d*ln(r))/(c^2+d^2)
    return Division(
            Subtraction(
              Multiplication(th, c).shallowReduce(context, angleUnit, ReductionTarget::BottomUpComputation),
              Multiplication(
                d,
                NaperianLogarithm::Builder(r).shallowReduce(context, angleUnit, ReductionTarget::BottomUpComputation)
              ).shallowReduce(context, angleUnit, ReductionTarget::BottomUpComputation)
            ).shallowReduce(context, angleUnit, ReductionTarget::BottomUpComputation),
            denominator
          ).shallowReduce(context, angleUnit, ReductionTarget::BottomUpComputation);
  }
}

Layout NthRootNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return NthRootLayout(
      childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits),
      childAtIndex(1)->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

int NthRootNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, NthRoot::s_functionHelper.name());
}

Expression NthRootNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, ReductionTarget target) {
  return NthRoot(this).shallowReduce(context, angleUnit, target);
}

template<typename T>
Evaluation<T> NthRootNode::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> base = childAtIndex(0)->approximate(T(), context, angleUnit);
  Evaluation<T> index = childAtIndex(1)->approximate(T(), context, angleUnit);
  Complex<T> result = Complex<T>::Undefined();
  if (base.type() == EvaluationNode<T>::Type::Complex
      && index.type() == EvaluationNode<T>::Type::Complex)
  {
    Complex<T> basec = static_cast<Complex<T> &>(base);
    Complex<T> indexc = static_cast<Complex<T> &>(index);
    result = PowerNode::compute(basec.stdComplex(), std::complex<T>(1)/(indexc.stdComplex()));
  }
  return result;
}

Expression NthRoot::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  {
    Expression e = Expression::defaultShallowReduce(context, angleUnit);
    if (e.isUndefined()) {
      return e;
    }
  }
#if MATRIX_EXACT_REDUCING
  if (childAtIndex(0).type() == ExpressionNode::Type::Matrix || childAtIndex(1).type() == ExpressionNode:Type::Matrix) {
    return Undefined();
  }
#endif
  Expression invIndex = Power(childAtIndex(1), Rational(-1));
  Power p = Power(childAtIndex(0), invIndex);
  invIndex.shallowReduce(context, angleUnit, target);
  replaceWithInPlace(p);
  return p.shallowReduce(context, angleUnit, target);
}

}
