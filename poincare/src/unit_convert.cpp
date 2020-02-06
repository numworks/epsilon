#include <poincare/unit_convert.h>
#include <poincare/complex.h>
#include <poincare/context.h>
#include <poincare/division.h>
#include <poincare/float.h>
#include <poincare/infinity.h>
#include <poincare/multiplication.h>
#include <poincare/symbol.h>
#include <poincare/undefined.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>

namespace Poincare {

Expression UnitConvertNode::shallowReduce(ReductionContext reductionContext) {
  return UnitConvert(this).shallowReduce(reductionContext);
}

template<typename T>
Evaluation<T> UnitConvertNode::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  /* If we are here, it means that the unit convert node was not shallowReduced.
   * Otherwise, it would have been replaced by the division of the value by the
   * unit. We thus return Undefined. */
  return Complex<T>::Undefined();
}

Expression UnitConvert::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }

  // Find the unit
  {
    ExpressionNode::ReductionContext reductionContextWithUnits = ExpressionNode::ReductionContext(
        reductionContext.context(),
        reductionContext.complexFormat(),
        reductionContext.angleUnit(),
        reductionContext.target(),
        ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithUndefinedAndReplaceUnits);
    Expression unit = childAtIndex(1).clone().reduce(reductionContextWithUnits).getUnit();
    if (unit.isUndefined()) {
      // There is no unit on the right
      return replaceWithUndefinedInPlace();
    }
  }

  ExpressionNode::ReductionContext reductionContextWithoutUnits = ExpressionNode::ReductionContext(
      reductionContext.context(),
      reductionContext.complexFormat(),
      reductionContext.angleUnit(),
      reductionContext.target(),
      ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithUndefinedAndDoNotReplaceUnits);
  Expression finalUnit = childAtIndex(1).reduce(reductionContextWithoutUnits).getUnit();

  // Divide the left member by the new unit
  Expression division = Division::Builder(childAtIndex(0), finalUnit.clone());
  division = division.reduce(reductionContext);
  if (division.hasUnit()) {
    // The left and right members are not homogeneous
    return replaceWithUndefinedInPlace();
  }
  double floatValue = division.approximateToScalar<double>(reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit());
  if (std::isinf(floatValue)) {
    return Infinity::Builder(false); //FIXME sign?
  }
  if (std::isnan(floatValue)) {
    return Undefined::Builder();
  }
  division = Multiplication::Builder(Float<double>::Builder(floatValue), finalUnit);
  static_cast<Multiplication &>(division).mergeMultiplicationChildrenInPlace();
  replaceWithInPlace(division);
  return division;
}

template Evaluation<float> UnitConvertNode::templatedApproximate<float>(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
template Evaluation<double> UnitConvertNode::templatedApproximate<double>(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;

}
