#include "expression_parameter_controller.h"

#include "poincare/expression.h"
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
  parameterAtIndex(row).writeText(buffer, precision,
                                  Preferences::PrintFloatMode::Decimal);
  textFieldOfCellAtIndex(cell, row)->setText(buffer);
}

// TODO: factorize duplicate code with FloatParameterController
bool ExpressionParameterController::textFieldDidFinishEditing(
    AbstractTextField* textField, Ion::Events::Event event) {
  char* text = textField->draftText();
  SerializedExpression currentExpression =
      SerializedExpression(Expression::Parse(text, App::app()->localContext()));
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
  return HasUndefinedValue(FloatType(value),
                           infTolerance == InfinityTolerance::PlusInfinity,
                           infTolerance == InfinityTolerance::MinusInfinity);
}

}  // namespace Shared
