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
  // 1. First rows: miscellaneous classic units for some dimensions
  Expression copy = m_expression.clone();
  Expression units;
  // Reduce to be able to recognize units
  PoincareHelpers::Reduce(&copy, App::app()->localContext(), ExpressionNode::ReductionTarget::User);
  copy = copy.removeUnit(&units);
  bool requireSimplification = false;
  bool canChangeUnitPrefix = false;

  if (Unit::IsSISpeed(units)) {
    // 1.a. Turn speed into km/h
    expressions[numberOfExpressions++] = UnitConvert::Builder(
        m_expression.clone(),
        Multiplication::Builder(
          Unit::Kilometer(),
          Power::Builder(
            Unit::Hour(),
            Rational::Builder(-1)
            )
          )
        );
    requireSimplification = true; // Simplify the conversion
  } else if (Unit::IsSIVolume(units)) {
    // 1.b. Turn volume into L
    expressions[numberOfExpressions++] = UnitConvert::Builder(
        m_expression.clone(),
        Unit::Liter()
        );
    requireSimplification = true; // Simplify the conversion
    canChangeUnitPrefix = true; // Pick best prefix (mL)
  } else if (Unit::IsSIEnergy(units)) {
    // 1.c. Turn energy into Wh
    expressions[numberOfExpressions++] = UnitConvert::Builder(
        m_expression.clone(),
        Multiplication::Builder(
          Unit::Watt(),
          Unit::Hour()
          )
        );
    expressions[numberOfExpressions++] = UnitConvert::Builder(
        m_expression.clone(),
        Unit::ElectronVolt()
        );
    requireSimplification = true; // Simplify the conversion
    canChangeUnitPrefix = true; // Pick best prefix (kWh)
  } else if (Unit::IsSITime(units)) {
    // Turn time into ? year + ? month + ? day + ? h + ? min + ? s
    double value = Shared::PoincareHelpers::ApproximateToScalar<double>(copy, App::app()->localContext());
    expressions[numberOfExpressions++] = Unit::BuildTimeSplit(value, App::app()->localContext(), Preferences::sharedPreferences()->complexFormat(), Preferences::sharedPreferences()->angleUnit());
  }
  // 1.d. Simplify and tune prefix of all computed expressions
  size_t currentExpressionIndex = 0;
  while (currentExpressionIndex < numberOfExpressions) {
    assert(!expressions[currentExpressionIndex].isUninitialized());
    if (requireSimplification) {
      Shared::PoincareHelpers::Simplify(&expressions[currentExpressionIndex], App::app()->localContext(), ExpressionNode::ReductionTarget::User);
    }
    if (canChangeUnitPrefix) {
      Expression newUnits;
      // Reduce to be able to removeUnit
      PoincareHelpers::Reduce(&expressions[currentExpressionIndex], App::app()->localContext(), ExpressionNode::ReductionTarget::User);
      expressions[currentExpressionIndex] = expressions[currentExpressionIndex].removeUnit(&newUnits);
      double value = Shared::PoincareHelpers::ApproximateToScalar<double>(expressions[currentExpressionIndex], App::app()->localContext());
      ExpressionNode::ReductionContext reductionContext(
          App::app()->localContext(),
          Preferences::sharedPreferences()->complexFormat(),
          Preferences::sharedPreferences()->angleUnit(),
          ExpressionNode::ReductionTarget::User,
          ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined);
      Unit::ChooseBestPrefixForValue(&newUnits, &value, reductionContext);
      expressions[currentExpressionIndex] = Multiplication::Builder(Number::FloatNumber(value), newUnits);
    }
    currentExpressionIndex++;
  }

  // 2. IS units only
  assert(numberOfExpressions < k_maxNumberOfRows - 1);
  expressions[numberOfExpressions] = m_expression.clone();
  Shared::PoincareHelpers::Simplify(&expressions[numberOfExpressions], App::app()->localContext(), ExpressionNode::ReductionTarget::User, Poincare::ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition, Poincare::ExpressionNode::UnitConversion::InternationalSystem);
  numberOfExpressions++;

  // 3. Get rid of duplicates
  Expression reduceExpression = m_expression.clone();
  // Make m_expression comparable to expressions (turn BasedInteger into Rational for instance)
  Shared::PoincareHelpers::Simplify(&reduceExpression, App::app()->localContext(), ExpressionNode::ReductionTarget::User, Poincare::ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition, Poincare::ExpressionNode::UnitConversion::None);
  currentExpressionIndex = 1;
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
