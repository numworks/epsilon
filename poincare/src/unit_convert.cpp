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
   * reduced expression. We do not return any unit as we do not want the caller
   * to believe the call to removeUnit has succeeded. */
  *unit = Expression();
  return UnitConvert(this).replaceWithUndefinedInPlace();
}

Expression UnitConvertNode::shallowBeautify(ReductionContext * reductionContext) {
  return UnitConvert(this).shallowBeautify(reductionContext);
}

template<typename T>
Evaluation<T> UnitConvertNode::templatedApproximate(ApproximationContext approximationContext) const {
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
      reductionContext.unitFormat(),
      reductionContext.target(),
      ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithUndefined,
      ExpressionNode::UnitConversion::None);
  // Don't transform the targeted unit
  childAtIndex(1).deepReduce(reductionContextKeepUnitAsIs);
}

Expression UnitConvert::deepBeautify(ExpressionNode::ReductionContext reductionContext) {
  Expression e = shallowBeautify(&reductionContext);
  ExpressionNode::ReductionContext reductionContextKeepUnitAsIs = ExpressionNode::ReductionContext(
      reductionContext.context(),
      reductionContext.complexFormat(),
      reductionContext.angleUnit(),
      reductionContext.unitFormat(),
      reductionContext.target(),
      reductionContext.symbolicComputation(),
      ExpressionNode::UnitConversion::None);
  SimplificationHelper::deepBeautifyChildren(e, reductionContextKeepUnitAsIs);
  return e;
}

Expression UnitConvert::shallowBeautify(ExpressionNode::ReductionContext * reductionContext) {
  // Discard cases like 4 -> _m/_km
  {
    ExpressionNode::ReductionContext reductionContextWithUnits = ExpressionNode::ReductionContext(
        reductionContext->context(),
        reductionContext->complexFormat(),
        reductionContext->angleUnit(),
        reductionContext->unitFormat(),
        reductionContext->target(),
        ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithUndefined);
    Expression unit;
    Expression childWithoutUnit = childAtIndex(1).clone().reduceAndRemoveUnit(reductionContextWithUnits, &unit);
    if (childWithoutUnit.isUndefined() || unit.isUninitialized()) {
      // There is no unit on the right
      return replaceWithUndefinedInPlace();
    }
  }
  // Find the unit
  Expression unit;
  childAtIndex(1).removeUnit(&unit);
  if (unit.isUninitialized()) {
    // There is no unit on the right
    return replaceWithUndefinedInPlace();
  }

  /* Handle temperatures, as converting between Kelvin, Celsius and Fahrenheit
   * cannot be done with a division. */
  if (unit.type() == ExpressionNode::Type::Unit) {
    Unit unitRef = static_cast<Unit &>(unit);
    if (unitRef.representative()->dimensionVector() == Unit::TemperatureRepresentative::Default().dimensionVector()) {
      Expression result = Unit::ConvertTemperatureUnits(childAtIndex(0), unitRef, *reductionContext);
      replaceWithInPlace(result);
      return result;
    }
  }

  // Divide the left member by the new unit
  Expression division = Division::Builder(childAtIndex(0), unit.clone());
  Expression divisionUnit;
  division = division.reduceAndRemoveUnit(*reductionContext, &divisionUnit);
  if (!divisionUnit.isUninitialized()) {
    // The left and right members are not homogeneous
    return replaceWithUndefinedInPlace();
  }
  Expression result = Multiplication::Builder(division, unit);
  replaceWithInPlace(result);
  ExpressionNode::ReductionContext reductionContextWithoutUnits = ExpressionNode::ReductionContext(
      reductionContext->context(),
      reductionContext->complexFormat(),
      reductionContext->angleUnit(),
      reductionContext->unitFormat(),
      reductionContext->target(),
      ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithUndefined,
      ExpressionNode::UnitConversion::None);
  result = result.shallowReduce(reductionContextWithoutUnits);
  result = result.shallowBeautify(&reductionContextWithoutUnits);
  *reductionContext = ExpressionNode::ReductionContext(
      reductionContext->context(),
      reductionContext->complexFormat(),
      reductionContext->angleUnit(),
      reductionContext->unitFormat(),
      reductionContext->target(),
      reductionContext->symbolicComputation(),
      ExpressionNode::UnitConversion::None);
  return result;
}

template Evaluation<float> UnitConvertNode::templatedApproximate<float>(ApproximationContext approximationContext) const;
template Evaluation<double> UnitConvertNode::templatedApproximate<double>(ApproximationContext approximationContext) const;

}
