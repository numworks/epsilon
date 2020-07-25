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

Expression UnitConvertNode::removeUnit(Expression * unit) {
  /* Warning: removeUnit of a UnitConvert doesn't make much sense but we
   * implement a 'dummy' version since UnitConvert still exists among the
   * reduced expression. */
  childAtIndex(1)->removeUnit(unit);
  return UnitConvert(this).replaceWithUndefinedInPlace();
}
Expression UnitConvertNode::shallowBeautify(ReductionContext reductionContext) {
  return UnitConvert(this).shallowBeautify(reductionContext);
}

void UnitConvertNode::deepReduceChildren(ExpressionNode::ReductionContext reductionContext) {
  UnitConvert(this).deepReduceChildren(reductionContext);
}

template<typename T>
Evaluation<T> UnitConvertNode::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  /* If we are here, it means that the unit convert node was not shallowReduced.
   * Otherwise, it would have been replaced by the division of the value by the
   * unit. We thus return Undefined. */
  return Complex<T>::Undefined();
}

void UnitConvert::deepReduceChildren(ExpressionNode::ReductionContext reductionContext) {
  childAtIndex(0).deepReduce(reductionContext);
  ExpressionNode::ReductionContext reductionContextKeepUnitAsIs = ExpressionNode::ReductionContext(
      reductionContext.context(),
      reductionContext.complexFormat(),
      reductionContext.angleUnit(),
      reductionContext.target(),
      ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithUndefined,
      ExpressionNode::UnitConversion::None);
  // Don't transform the targetted unit
  childAtIndex(1).deepReduce(reductionContextKeepUnitAsIs);
}

Expression UnitConvert::shallowBeautify(ExpressionNode::ReductionContext reductionContext) {
  // Discard cases like 4 -> _m/_km
  {
    ExpressionNode::ReductionContext reductionContextWithUnits = ExpressionNode::ReductionContext(
        reductionContext.context(),
        reductionContext.complexFormat(),
        reductionContext.angleUnit(),
        reductionContext.target(),
        ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithUndefined);
    Expression unit;
    childAtIndex(1).clone().deepReduce(reductionContextWithUnits).removeUnit(&unit);
    if (unit.isUninitialized()) {
      // There is no unit on the right
      return replaceWithUndefinedInPlace();
    }
  }
  // Find the unit
  ExpressionNode::ReductionContext reductionContextWithoutUnits = ExpressionNode::ReductionContext(
      reductionContext.context(),
      reductionContext.complexFormat(),
      reductionContext.angleUnit(),
      reductionContext.target(),
      ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithUndefined,
      ExpressionNode::UnitConversion::None);
  Expression unit;
  childAtIndex(1).removeUnit(&unit);
  if (unit.isUninitialized()) {
    // There is no unit on the right
    return replaceWithUndefinedInPlace();
  }

  // Divide the left member by the new unit
  Expression division = Division::Builder(childAtIndex(0), unit.clone());
  division = division.deepReduce(reductionContext);
  Expression divisionUnit;
  division = division.removeUnit(&divisionUnit);
  if (!divisionUnit.isUninitialized()) {
    // The left and right members are not homogeneous
    return replaceWithUndefinedInPlace();
  }
  Expression result = Multiplication::Builder(division, unit);
  replaceWithInPlace(result);
  result.shallowReduce(reductionContextWithoutUnits);
  return result.shallowBeautify(reductionContextWithoutUnits);
}

template Evaluation<float> UnitConvertNode::templatedApproximate<float>(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
template Evaluation<double> UnitConvertNode::templatedApproximate<double>(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;

}
