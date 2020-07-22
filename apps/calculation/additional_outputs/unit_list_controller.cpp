#include "unit_list_controller.h"
#include "../app.h"
#include "../../shared/poincare_helpers.h"
#include <poincare/unit_convert.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>
#include <poincare/undefined.h>
#include <poincare/unit.h>

using namespace Poincare;
using namespace Shared;

namespace Calculation {

void storeAndSimplify(Expression e, Expression * dest, bool requireSimplification, bool canChangeUnitPrefix, ExpressionNode::ReductionContext reductionContext) {
  assert(!e.isUninitialized());
  *dest = e;
  if (requireSimplification) {
    *dest = dest->simplify(reductionContext);
  }
  if (canChangeUnitPrefix) {
    Expression newUnits;
    /* If the expression has already been simplified, we do not want to reduce
     * it further, as this would units introduced by a UnitConvert node back to
     * SI units. */
    if (!requireSimplification) {
      // Reduce to be able to removeUnit
      *dest = dest->reduce(reductionContext);
    }
    *dest = dest->removeUnit(&newUnits);
    double value = Shared::PoincareHelpers::ApproximateToScalar<double>(*dest, App::app()->localContext());
    *dest = Multiplication::Builder(Number::FloatNumber(value), newUnits);
  }
}

void UnitListController::setExpression(Poincare::Expression e) {
  ExpressionsListController::setExpression(e);
  assert(!m_expression.isUninitialized());
  static_assert(k_maxNumberOfRows >= 3, "k_maxNumberOfRows must be greater than 3");

  Poincare::Expression expressions[k_maxNumberOfRows];
  // Initialize expressions
  for (size_t i = 0; i < k_maxNumberOfRows; i++) {
    expressions[i] = Expression();
  }

  size_t numberOfExpressions = 0;
  /* 1. First rows: miscellaneous classic units for some dimensions, in both
   * metric and imperial units. */
  Expression copy = m_expression.clone();
  Expression units;
  // Reduce to be able to recognize units
  PoincareHelpers::Reduce(&copy, App::app()->localContext(), ExpressionNode::ReductionTarget::User);
  copy = copy.removeUnit(&units);
  Preferences::UnitFormat chosenFormat = GlobalPreferences::sharedGlobalPreferences()->unitFormat();
  Preferences::UnitFormat otherFormat = (chosenFormat == Preferences::UnitFormat::Metric) ? Preferences::UnitFormat::Imperial : Preferences::UnitFormat::Metric;
  ExpressionNode::ReductionContext chosenFormatContext(
      App::app()->localContext(),
      Preferences::sharedPreferences()->complexFormat(),
      Preferences::sharedPreferences()->angleUnit(),
      chosenFormat,
      ExpressionNode::ReductionTarget::User,
      ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined);
  ExpressionNode::ReductionContext otherFormatContext(
      App::app()->localContext(),
      Preferences::sharedPreferences()->complexFormat(),
      Preferences::sharedPreferences()->angleUnit(),
      otherFormat,
      ExpressionNode::ReductionTarget::User,
      ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined);

  if (Unit::IsSISpeed(units)) {
    // 1.a. Turn speed into km/h or mi/h
    storeAndSimplify(Unit::StandardSpeedConversion(m_expression.clone(), chosenFormatContext), &expressions[numberOfExpressions++], true, false, chosenFormatContext);
    storeAndSimplify(Unit::StandardSpeedConversion(m_expression.clone(), otherFormatContext), &expressions[numberOfExpressions++], true, false, otherFormatContext);
  } else if (Unit::IsSIVolume(units)) {
    // 1.b. Turn volume into L or _gal + _cp + _floz
    storeAndSimplify(Unit::StandardVolumeConversion(m_expression.clone(), chosenFormatContext), &expressions[numberOfExpressions++], chosenFormat == Preferences::UnitFormat::Metric, chosenFormat == Preferences::UnitFormat::Metric, chosenFormatContext);
    storeAndSimplify(Unit::StandardVolumeConversion(m_expression.clone(), otherFormatContext), &expressions[numberOfExpressions++], otherFormat == Preferences::UnitFormat::Metric, otherFormat == Preferences::UnitFormat::Metric, otherFormatContext);
  } else if (Unit::IsSIEnergy(units)) {
    // 1.c. Turn energy into Wh
    storeAndSimplify(UnitConvert::Builder(m_expression.clone(), Multiplication::Builder(Unit::Watt(), Unit::Hour())), &expressions[numberOfExpressions++], true, true, chosenFormatContext);
    storeAndSimplify(UnitConvert::Builder(m_expression.clone(), Unit::ElectronVolt()), &expressions[numberOfExpressions++], true, true, chosenFormatContext);
  } else if (Unit::IsSITime(units)) {
    // 1.d. Turn time into ? year + ? month + ? day + ? h + ? min + ? s
    double value = Shared::PoincareHelpers::ApproximateToScalar<double>(copy, App::app()->localContext());
    expressions[numberOfExpressions++] = Unit::BuildTimeSplit(value, App::app()->localContext());
  } else if (Unit::IsSIDistance(units)) {
    // 1.e. Turn distance into _?m or _mi + _yd + _ft + _in
    storeAndSimplify(Unit::StandardDistanceConversion(m_expression.clone(), chosenFormatContext), &expressions[numberOfExpressions++], chosenFormat == Preferences::UnitFormat::Metric, chosenFormat == Preferences::UnitFormat::Metric, chosenFormatContext);
    storeAndSimplify(Unit::StandardDistanceConversion(m_expression.clone(), otherFormatContext), &expressions[numberOfExpressions++], otherFormat == Preferences::UnitFormat::Metric, otherFormat == Preferences::UnitFormat::Metric, otherFormatContext);
  } else if (Unit::IsSISurface(units)) {
    // 1.f. Turn surface into hectares or acres
    storeAndSimplify(Unit::StandardSurfaceConversion(m_expression.clone(), chosenFormatContext), &expressions[numberOfExpressions++], true, false, chosenFormatContext);
    storeAndSimplify(Unit::StandardSurfaceConversion(m_expression.clone(), otherFormatContext), &expressions[numberOfExpressions++], true, false, otherFormatContext);
  } else if (Unit::IsSIMass(units)) {
    // 1.g. Turn mass into _?g and _lb + _oz
    storeAndSimplify(Unit::StandardMassConversion(m_expression.clone(), chosenFormatContext), &expressions[numberOfExpressions++], chosenFormat == Preferences::UnitFormat::Metric, chosenFormat == Preferences::UnitFormat::Metric, chosenFormatContext);
    storeAndSimplify(Unit::StandardMassConversion(m_expression.clone(), otherFormatContext), &expressions[numberOfExpressions++], otherFormat == Preferences::UnitFormat::Metric, otherFormat == Preferences::UnitFormat::Metric, otherFormatContext);
  }

  // 2. SI units only
  assert(numberOfExpressions < k_maxNumberOfRows - 1);
  expressions[numberOfExpressions] = m_expression.clone();
  Shared::PoincareHelpers::Simplify(&expressions[numberOfExpressions], App::app()->localContext(), ExpressionNode::ReductionTarget::User, Poincare::ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition, Poincare::ExpressionNode::UnitConversion::InternationalSystem);
  numberOfExpressions++;

  // 3. Get rid of duplicates
  Expression reduceExpression = m_expression.clone();
  // Make m_expression comparable to expressions (turn BasedInteger into Rational for instance)
  Shared::PoincareHelpers::Simplify(&reduceExpression, App::app()->localContext(), ExpressionNode::ReductionTarget::User, Poincare::ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition, Poincare::ExpressionNode::UnitConversion::None);
  int currentExpressionIndex = 1;
  while (currentExpressionIndex < numberOfExpressions) {
    bool duplicateFound = false;
    for (size_t i = 0; i < currentExpressionIndex + 1; i++) {
      // Compare the currentExpression to all previous expressions and to m_expression
      Expression comparedExpression = i == currentExpressionIndex ? reduceExpression : expressions[i];
      assert(!comparedExpression.isUninitialized());
      if (comparedExpression.isIdenticalTo(expressions[currentExpressionIndex])) {
        numberOfExpressions--;
        // Shift next expressions
        for (size_t j = currentExpressionIndex; j < numberOfExpressions; j++) {
          expressions[j] = expressions[j+1];
        }
        // Remove last expression
        expressions[numberOfExpressions] = Expression();
        // The current expression has been discarded, no need to increment the current index
        duplicateFound = true;
        break;
      }
    }
    if (!duplicateFound) {
      // The current expression is not a duplicate, check next expression
      currentExpressionIndex++;
    }
  }
  // Memoize layouts
  for (size_t i = 0; i < k_maxNumberOfRows; i++) {
    if (!expressions[i].isUninitialized()) {
      m_layouts[i] = Shared::PoincareHelpers::CreateLayout(expressions[i]);
    }
  }
}

I18n::Message UnitListController::messageAtIndex(int index) {
  return (I18n::Message)0;
}

}
