#include <poincare/nth_root.h>
#include <poincare/approximation_helper.h>
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
#include <poincare/simplification_helper.h>
#include <assert.h>
#include <cmath>
#include <utility>

namespace Poincare {

int NthRootNode::numberOfChildren() const { return NthRoot::s_functionHelper.numberOfChildren(); }

Layout NthRootNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return NthRootLayout::Builder(
      childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits),
      childAtIndex(1)->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

int NthRootNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, NthRoot::s_functionHelper.name());
}

Expression NthRootNode::shallowReduce(ReductionContext reductionContext) {
  return NthRoot(this).shallowReduce(reductionContext);
}

template<typename T>
Evaluation<T> NthRootNode::templatedApproximate(ApproximationContext approximationContext) const {
  return ApproximationHelper::Map<T>(
      this,
      approximationContext,
      [] (const std::complex<T> * c, int numberOfComplexes, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, void * ctx) {
        assert(numberOfComplexes == 2);
        std::complex<T> basec = c[0];
        std::complex<T> indexc = c[1];
        /* If the complexFormat is Real, we look for nthroot of form root(x,q) with
         * x real and q integer because they might have a real form which does not
         * correspond to the principale angle. */
        if (complexFormat == Preferences::ComplexFormat::Real && indexc.imag() == 0.0 && std::round(indexc.real()) == indexc.real()) {
          // root(x, q) with q integer and x real
          Complex<T> result = PowerNode::computeNotPrincipalRealRootOfRationalPow(basec, static_cast<T>(1.0), indexc.real());
          if (!result.isUndefined()) {
            return result;
          }
        }
        return PowerNode::computeOnComplex<T>(basec, std::complex<T>(1.0)/(indexc), complexFormat);
      });
}

Expression NthRoot::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::shallowReduceUndefined(*this);
    if (!e.isUninitialized()) {
      return e;
    }
    e = SimplificationHelper::undefinedOnMatrix(*this, reductionContext);
    if (!e.isUninitialized()) {
      return e;
    }
    e = SimplificationHelper::distributeReductionOverLists(*this, reductionContext);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  Expression invIndex = Power::Builder(childAtIndex(1), Rational::Builder(-1));
  Power p = Power::Builder(childAtIndex(0), invIndex);
  invIndex.shallowReduce(reductionContext);
  replaceWithInPlace(p);
  return p.shallowReduce(reductionContext);
}

}
