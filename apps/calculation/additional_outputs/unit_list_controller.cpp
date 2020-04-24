#include "unit_list_controller.h"
#include "../app.h"
#include "../../shared/poincare_helpers.h"
#include <poincare/unit_convert.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>
#include <poincare/undefined.h>
#include <poincare/unit.h>

using namespace Poincare;

namespace Calculation {

int UnitListController::numberOfRows() const { //FIXME
  return 2;
}

void UnitListController::computeLayoutAtIndex(int index) { //FIXME
  Expression expressionAtIndex = m_expression.clone();
  if (index == 0) {
    // SI units only
    Shared::PoincareHelpers::Simplify(&expressionAtIndex, App::app()->localContext(), ExpressionNode::ReductionTarget::User, Poincare::ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition, Poincare::ExpressionNode::UnitConversion::InternationalSystem);
  } else if (index == 1) {
    ExpressionNode::ReductionContext reductionContext(
        App::app()->localContext(),
        Preferences::sharedPreferences()->complexFormat(),
        Preferences::sharedPreferences()->angleUnit(),
        ExpressionNode::ReductionTarget::User,
        ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined);
    Expression units;
    // Reduce to be able to recognize units
    expressionAtIndex = expressionAtIndex.reduce(reductionContext);
    expressionAtIndex = expressionAtIndex.removeUnit(&units);
    bool requireSimplification = true;
    bool canChangeUnitPrefix = true;
    if (Unit::IsISSpeed(units)) {
      expressionAtIndex = UnitConvert::Builder(
          m_expression.clone(),
          Multiplication::Builder(
            Unit::Kilometer(),
            Power::Builder(
              Unit::Hour(),
              Rational::Builder(-1)
              )
            )
          );
      canChangeUnitPrefix = false; // We force km/h
    } else if (Unit::IsISVolume(units)) {
      expressionAtIndex = UnitConvert::Builder(
          m_expression.clone(),
          Unit::Liter()
          );
    } else if (Unit::IsISEnergy(units)) {
      expressionAtIndex = UnitConvert::Builder(
          m_expression.clone(),
          Multiplication::Builder(
              Unit::Watt(),
              Unit::Hour()
            )
          );
    } else if (Unit::IsISTime(units)) {
      double value = Shared::PoincareHelpers::ApproximateToScalar<double>(expressionAtIndex, App::app()->localContext());
      expressionAtIndex = Unit::BuildTimeSplit(value);
      requireSimplification = false;
      canChangeUnitPrefix = false;
    } else {
      requireSimplification = false;
      canChangeUnitPrefix = false;
    }

    if (requireSimplification) {
      Shared::PoincareHelpers::Simplify(&expressionAtIndex, App::app()->localContext(), ExpressionNode::ReductionTarget::User);
    }
    if (canChangeUnitPrefix) {
      Expression newUnits;
      expressionAtIndex = expressionAtIndex.removeUnit(&newUnits);
      double value = Shared::PoincareHelpers::ApproximateToScalar<double>(expressionAtIndex, App::app()->localContext());
      Unit::ChooseBestMultipleForValue(&newUnits, &value, reductionContext);
      expressionAtIndex = Multiplication::Builder(Number::FloatNumber(value), newUnits);
    }
  }
  m_layouts[index] = Shared::PoincareHelpers::CreateLayout(expressionAtIndex);
}

I18n::Message UnitListController::messageAtIndex(int index) {
  return (I18n::Message)0;
}

}
