#include "float_parameter_controller.h"

#include "poincare_helpers.h"

using namespace Escher;
using namespace Poincare;

namespace Shared {

template <typename T>
FloatParameterController<T>::FloatParameterController(
    Responder* parentResponder, View* topView)
    : ParametersWithValidationController(parentResponder, topView) {}

template <typename T>
void FloatParameterController<T>::fillCellForRow(HighlightCell* cell, int row) {
  if (typeAtRow(row) == k_buttonCellType ||
      textFieldOfCellAtIndex(cell, row)->isEditing()) {
    return;
  }
  constexpr int precision = Preferences::VeryLargeNumberOfSignificantDigits;
  constexpr int bufferSize =
      PrintFloat::charSizeForFloatsWithPrecision(precision);
  char buffer[bufferSize];
  PoincareHelpers::ConvertFloatToTextWithDisplayMode<FloatType>(
      parameterAtIndex(row), buffer, bufferSize, precision,
      Preferences::PrintFloatMode::Decimal);
  textFieldOfCellAtIndex(cell, row)->setText(buffer);
}

template <typename T>
bool FloatParameterController<T>::textFieldDidFinishEditing(
    AbstractTextField* textField, Ion::Events::Event event) {
  char* text = textField->draftText();
  FloatType floatBody = ParseInputFloatValue<T>(text);
  if (hasUndefinedValue(text, floatBody, innerSelectedRow()) ||
      !setParameterAtIndex(innerSelectedRow(), floatBody)) {
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

template <typename T>
bool FloatParameterController<T>::hasUndefinedValue(const char* text,
                                                    ParameterType value,
                                                    int row) const {
  InfinityTolerance infTolerance = infinityAllowanceForRow(row);
  return HasUndefinedValue(value,
                           infTolerance == InfinityTolerance::PlusInfinity,
                           infTolerance == InfinityTolerance::MinusInfinity);
}

template class FloatParameterController<float>;
template class FloatParameterController<double>;

}  // namespace Shared
