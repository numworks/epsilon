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

Layout NthRootNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return NthRootLayout::Builder(
      childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits),
      childAtIndex(1)->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

int NthRootNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, NthRoot::s_functionHelper.name());
}

Expression NthRootNode::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target, bool symbolicComputation) {
  return NthRoot(this).shallowReduce(context, complexFormat, angleUnit, target);
}

template<typename T>
Evaluation<T> NthRootNode::templatedApproximate(Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> base = childAtIndex(0)->approximate(T(), context, complexFormat, angleUnit);
  Evaluation<T> index = childAtIndex(1)->approximate(T(), context, complexFormat, angleUnit);
  Complex<T> result = Complex<T>::Undefined();
  if (base.type() == EvaluationNode<T>::Type::Complex
      && index.type() == EvaluationNode<T>::Type::Complex)
  {
    std::complex<T> basec = static_cast<Complex<T> &>(base).stdComplex();
    std::complex<T> indexc = static_cast<Complex<T> &>(index).stdComplex();
    /* If the complexFormat is Real, we look for nthroot of form root(x,q) with
     * x real and q integer because they might have a real form which does not
     * correspond to the principale angle. */
    if (complexFormat == Preferences::ComplexFormat::Real) {
      // root(x, q) with q integer and x real
      if (basec.imag() == 0.0 && indexc.imag() == 0.0 && std::round(indexc.real()) == indexc.real()) {
        std::complex<T> absBasec = basec;
        absBasec.real(std::fabs(absBasec.real()));
        // compute root(|x|, q)
        Complex<T> absBasePowIndex = PowerNode::compute(absBasec, std::complex<T>(1.0)/(indexc), complexFormat);
        // q odd if (-1)^q = -1
        if (std::pow((T)-1.0, (T)indexc.real()) < 0.0) {
          return basec.real() < 0 ? Complex<T>::Builder(-absBasePowIndex.stdComplex()) : absBasePowIndex;
        }
      }
    }
    result = PowerNode::compute(basec, std::complex<T>(1.0)/(indexc), complexFormat);
  }
  return result;
}


Expression NthRoot::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }
#if MATRIX_EXACT_REDUCING
  if (childAtIndex(0).type() == ExpressionNode::Type::Matrix || childAtIndex(1).type() == ExpressionNode:Type::Matrix) {
    return Undefined::Builder();
  }
#endif
  Expression invIndex = Power::Builder(childAtIndex(1), Rational::Builder(-1));
  Power p = Power::Builder(childAtIndex(0), invIndex);
  invIndex.shallowReduce(context, complexFormat, angleUnit, target);
  replaceWithInPlace(p);
  return p.shallowReduce(context, complexFormat, angleUnit, target);
}

}
