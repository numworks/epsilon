#include <poincare/nth_root.h>
#include <poincare/division.h>
#include <poincare/power.h>
#include <poincare/undefined.h>
#include <poincare/nth_root_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <assert.h>
#include <cmath>

namespace Poincare {

constexpr Expression::FunctionHelper NthRoot::s_functionHelper;

int NthRootNode::numberOfChildren() const { return NthRoot::s_functionHelper.numberOfChildren(); }

Layout NthRootNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return NthRootLayout(
      childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits),
      childAtIndex(1)->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

int NthRootNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, NthRoot::s_functionHelper.name());
}

Expression NthRootNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {
  return NthRoot(this).shallowReduce(context, angleUnit, replaceSymbols);
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

Expression NthRoot::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {
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
  invIndex.shallowReduce(context, angleUnit);
  replaceWithInPlace(p);
  return p.shallowReduce(context, angleUnit);
}

}
