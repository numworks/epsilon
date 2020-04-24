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
    // Miscellaneous classic units for some dimensions
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
    bool requireSimplification = false;
    bool canChangeUnitPrefix = false;
    if (Unit::IsISSpeed(units)) {
      // Turn into km/h
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
      requireSimplification = true; // Reduce the conversion
    } else if (Unit::IsISVolume(units)) {
      // Turn into L
      expressionAtIndex = UnitConvert::Builder(
          m_expression.clone(),
          Unit::Liter()
          );
      requireSimplification = true; // reduce the conversion
      canChangeUnitPrefix = true; // Pick best prefix (mL)
    } else if (Unit::IsISEnergy(units)) {
      // Turn into W
      expressionAtIndex = UnitConvert::Builder(
          m_expression.clone(),
          Multiplication::Builder(
              Unit::Watt(),
              Unit::Hour()
            )
          );
      requireSimplification = true; // reduce the conversion
      canChangeUnitPrefix = true; // Pick best prefix (kWh)
    } else if (Unit::IsISTime(units)) {
      // Turn into ? year + ? month + ? day + ? h + ? min + ? s
      double value = Shared::PoincareHelpers::ApproximateToScalar<double>(expressionAtIndex, App::app()->localContext());
      expressionAtIndex = Unit::BuildTimeSplit(value);
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
