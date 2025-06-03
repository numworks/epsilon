#include "expression_parameter_controller.h"

#include <poincare/expression.h>

#include "poincare/expression_or_float.h"
#include "poincare/preferences.h"
#include "poincare_helpers.h"

using namespace Escher;
using namespace Poincare;

namespace Shared {

ExpressionParameterController::ExpressionParameterController(
    Responder* parentResponder, View* topView)
    : ParametersWithValidationController(parentResponder, topView) {}

// TODO: factorize duplicate code with FloatParameterController
void ExpressionParameterController::fillCellForRow(HighlightCell* cell,
                                                   int row) {
  if (typeAtRow(row) == k_buttonCellType ||
      textFieldOfCellAtIndex(cell, row)->isEditing()) {
    return;
  }
  constexpr int precision = Preferences::VeryLargeNumberOfSignificantDigits;
  char buffer[PrintFloat::charSizeForFloatsWithPrecision(precision)];
  parameterAtIndex(row).writeText(
      buffer,
      ExpressionOrFloat::ApproximationParameters{
          Preferences::SharedPreferences()->angleUnit(),
          Preferences::SharedPreferences()->complexFormat()},
      precision, Preferences::PrintFloatMode::Decimal);
  textFieldOfCellAtIndex(cell, row)->setText(buffer);
}

// TODO: factorize duplicate code with FloatParameterController
bool ExpressionParameterController::textFieldDidFinishEditing(
    AbstractTextField* textField, Ion::Events::Event event) {
  const char* text = textField->draftText();
  UserExpression parsedExpression =
      Expression::Parse(text, App::app()->localContext());
  if (!parsedExpression.isUninitialized() &&
      !Poincare::Dimension(parsedExpression).isScalar()) {
    App::app()->displayWarning(I18n::Message::UndefinedValue);
    return false;
  }
  ExpressionOrFloat currentExpression = ExpressionOrFloat::Builder(
      parsedExpression, [](UserExpression expression) {
        return expression.approximateToRealScalar<float>(
            Preferences::SharedPreferences()->angleUnit(),
            Preferences::SharedPreferences()->complexFormat());
      });
  if (hasUndefinedValue(text, currentExpression, innerSelectedRow()) ||
      !setParameterAtIndex(innerSelectedRow(), currentExpression)) {
    return false;
  }
  m_selectableListView.reloadSelectedCell();
  m_selectableListView.reloadData();
  if (event == Ion::Events::EXE || event == Ion::Events::OK) {
    assert(innerSelectedRow() + 1 < numberOfRows());
    m_selectableListView.selectCell(selectedRow() + 1);
  } else {
    m_selectableListView.handleEvent(event);
  }
  return true;
}

bool ExpressionParameterController::hasUndefinedValue(const char* text,
                                                      ParameterType value,
                                                      int row) const {
  InfinityTolerance infTolerance = infinityAllowanceForRow(row);
  return HasUndefinedValue(PoincareHelpers::ToFloat(value),
                           infTolerance == InfinityTolerance::PlusInfinity,
                           infTolerance == InfinityTolerance::MinusInfinity);
}

}  // namespace Shared
