#include <assert.h>
#include <math.h>
#include <poincare/old/complex.h>
#include <poincare/old/context.h>
#include <poincare/old/division.h>
#include <poincare/old/float.h>
#include <poincare/old/infinity.h>
#include <poincare/old/multiplication.h>
#include <poincare/old/power.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/symbol.h>
#include <poincare/old/undefined.h>
#include <poincare/old/unit_convert.h>
#include <stdlib.h>

namespace Poincare {

OExpression UnitConvertNode::removeUnit(OExpression* unit) {
  /* Warning: removeUnit of a UnitConvert doesn't make much sense but we
   * implement a 'dummy' version since UnitConvert still exists among the
   * reduced expression. We do not return any unit as we do not want the caller
   * to believe the call to removeUnit has succeeded. */
  *unit = OExpression();
  return UnitConvert(this).replaceWithUndefinedInPlace();
}

OExpression UnitConvertNode::shallowBeautify(
    const ReductionContext& reductionContext) {
  return UnitConvert(this).shallowBeautify(reductionContext);
}

template <typename T>
Evaluation<T> UnitConvertNode::templatedApproximate(
    const ApproximationContext& approximationContext) const {
  /* If we are here, it means that the unit convert node was not shallowReduced.
   * Otherwise, it would have been replaced by the division of the value by the
   * unit. We thus return Undefined. */
  return Complex<T>::Undefined();
}

void UnitConvert::deepReduceChildren(const ReductionContext& reductionContext) {
  childAtIndex(0).deepReduce(reductionContext);
  ReductionContext childContext = reductionContext;
  childContext.setSymbolicComputation(
      SymbolicComputation::ReplaceAllSymbolsWithUndefined);
  childContext.setUnitConversion(UnitConversion::None);
  // Don't transform the targeted unit
  childAtIndex(1).deepReduce(childContext);
}

OExpression UnitConvert::deepBeautify(
    const ReductionContext& reductionContext) {
  OExpression e = shallowBeautify(reductionContext);
  ReductionContext childContext = reductionContext;
  childContext.setUnitConversion(UnitConversion::None);
  SimplificationHelper::deepBeautifyChildren(e, childContext);
  return e;
}

OExpression UnitConvert::shallowBeautify(
    const ReductionContext& reductionContext) {
  // Discard cases like 4 -> _m/_km
  {
    ReductionContext reductionContextWithUnits = reductionContext;
    reductionContextWithUnits.setSymbolicComputation(
        SymbolicComputation::ReplaceAllSymbolsWithUndefined);
    OExpression unit;
    OExpression childWithoutUnit = childAtIndex(1).cloneAndReduceAndRemoveUnit(
        reductionContextWithUnits, &unit);
    if (childWithoutUnit.isUndefined() || unit.isUninitialized()) {
      // There is no unit on the right
      return replaceWithUndefinedInPlace();
    }
  }
  // Find the unit
  OExpression unit;
  childAtIndex(1).removeUnit(&unit);
  if (unit.isUninitialized()) {
    // There is no unit on the right
    return replaceWithUndefinedInPlace();
  }

  /* Handle temperatures, as converting between Kelvin, Celsius and Fahrenheit
   * cannot be done with a division. */
  if (unit.otype() == ExpressionNode::Type::OUnit) {
    OUnit unitRef = static_cast<OUnit&>(unit);
    if (unitRef.representative()->dimensionVector() ==
        OUnit::TemperatureRepresentative::Default().dimensionVector()) {
      OExpression result = OUnit::ConvertTemperatureUnits(
          childAtIndex(0), unitRef, reductionContext);
      replaceWithInPlace(result);
      return result;
    }
  }

  // Divide the left member by the new unit
  OExpression divisionUnit;
  OExpression division =
      Division::Builder(childAtIndex(0), unit.clone())
          .cloneAndReduceAndRemoveUnit(reductionContext, &divisionUnit);
  if (!divisionUnit.isUninitialized()) {
    if (unit.isPureAngleUnit()) {
      // Try again with the current angle unit
      OUnit currentAngleUnit = OUnit::Builder(
          UnitNode::AngleRepresentative::DefaultRepresentativeForAngleUnit(
              reductionContext.angleUnit()));
      division =
          Multiplication::Builder(division, divisionUnit, currentAngleUnit);
      divisionUnit = OExpression();
      division =
          division.cloneAndReduceAndRemoveUnit(reductionContext, &divisionUnit);
      if (!divisionUnit.isUninitialized()) {
        return replaceWithUndefinedInPlace();
      }
    } else {
      // The left and right members are not homogeneous
      return replaceWithUndefinedInPlace();
    }
  }
  OExpression result = Multiplication::Builder(division, unit);
  replaceWithInPlace(result);
  ReductionContext childContext = reductionContext;
  childContext.setSymbolicComputation(
      SymbolicComputation::ReplaceAllSymbolsWithUndefined);
  childContext.setUnitConversion(UnitConversion::None);
  result = result.shallowReduce(childContext);
  result = result.shallowBeautify(childContext);
  return result;
}

template Evaluation<float> UnitConvertNode::templatedApproximate<float>(
    const ApproximationContext& approximationContext) const;
template Evaluation<double> UnitConvertNode::templatedApproximate<double>(
    const ApproximationContext& approximationContext) const;

}  // namespace Poincare
