#include <assert.h>
#include <poincare/layout.h>
#include <poincare/old/addition.h>
#include <poincare/old/approximation_helper.h>
#include <poincare/old/constant.h>
#include <poincare/old/division.h>
#include <poincare/old/naperian_logarithm.h>
#include <poincare/old/nth_root.h>
#include <poincare/old/power.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/subtraction.h>
#include <poincare/old/undefined.h>

#include <cmath>
#include <utility>

namespace Poincare {

int NthRootNode::numberOfChildren() const {
  return NthRoot::s_functionHelper.numberOfChildren();
}

size_t NthRootNode::serialize(char* buffer, size_t bufferSize,
                              Preferences::PrintFloatMode floatDisplayMode,
                              int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      NthRoot::s_functionHelper.aliasesList().mainAlias());
}

OExpression NthRootNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return NthRoot(this).shallowReduce(reductionContext);
}

template <typename T>
Evaluation<T> NthRootNode::templatedApproximate(
    const ApproximationContext& approximationContext) const {
  return ApproximationHelper::Map<T>(
      this, approximationContext,
      [](const std::complex<T>* c, int numberOfComplexes,
         Preferences::ComplexFormat complexFormat,
         Preferences::AngleUnit angleUnit, void* ctx) -> std::complex<T> {
        assert(numberOfComplexes == 2);
        std::complex<T> basec = c[0];
        std::complex<T> indexc = c[1];
        /* If the complexFormat is Real, we look for nthroot of form root(x,q)
         * with x real and q integer because they might have a real form which
         * does not correspond to the principale angle. */
        if (complexFormat == Preferences::ComplexFormat::Real &&
            indexc.imag() == 0.0 &&
            std::round(indexc.real()) == indexc.real()) {
          // root(x, q) with q integer and x real
          std::complex<T> result =
              PowerNode::computeNotPrincipalRealRootOfRationalPow(
                  basec, static_cast<T>(1.0), indexc.real());
          if (!std::isnan(result.real()) && !std::isnan(result.imag())) {
            return result;
          }
        }
        return PowerNode::computeOnComplex<T>(
            basec, std::complex<T>(1.0) / (indexc), complexFormat);
      });
}

OExpression NthRoot::shallowReduce(ReductionContext reductionContext) {
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
  OExpression invIndex = Power::Builder(childAtIndex(1), Rational::Builder(-1));
  Power p = Power::Builder(childAtIndex(0), invIndex);
  invIndex.shallowReduce(reductionContext);
  replaceWithInPlace(p);
  return p.shallowReduce(reductionContext);
}

}  // namespace Poincare
