#include "float_parameter_controller.h"

#include <assert.h>
#include <poincare/preferences.h>

#include <cmath>

#include "poincare_helpers.h"
#include "text_field_delegate_app.h"

using namespace Escher;
using namespace Poincare;

namespace Shared {

template <typename T>
FloatParameterController<T>::FloatParameterController(
    Responder *parentResponder)
    : SelectableListViewController(parentResponder),
      m_okButton(
          &m_selectableListView, I18n::Message::Ok,
          Invocation::Builder<FloatParameterController>(
              [](FloatParameterController *parameterController, void *sender) {
                parameterController->buttonAction();
                return true;
              },
              this)) {}

template <typename T>
void FloatParameterController<T>::didBecomeFirstResponder() {
  if (selectedRow() >= 0) {
    int selRow = selectedRow();
    selRow = selRow >= numberOfRows() ? numberOfRows() - 1 : selRow;
    selectCell(selRow);
  }
  SelectableListViewController<
      MemoizedListViewDataSource>::didBecomeFirstResponder();
}

template <typename T>
void FloatParameterController<T>::viewWillAppear() {
  ViewController::viewWillAppear();
  int selRow = selectedRow();
  if (selRow == -1 || typeAtIndex(selRow) == k_buttonCellType) {
    selectCell(0);
  } else {
    selRow = selRow >= numberOfRows() ? numberOfRows() - 1 : selRow;
    selectCell(selRow);
  }
  resetMemoization();
  m_selectableListView.reloadData();
}

template <typename T>
void FloatParameterController<T>::viewDidDisappear() {
  if (parentResponder() == nullptr) {
    m_selectableListView.deselectTable();
    m_selectableListView.scrollToCell(0);
  }
}

template <typename T>
bool FloatParameterController<T>::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Back) {
    stackController()->pop();
    return true;
  }
  return false;
}

template <typename T>
int FloatParameterController<T>::typeAtIndex(int index) const {
  if (index == this->numberOfRows() - 1) {
    return k_buttonCellType;
  }
  return k_parameterCellType;
}

template <typename T>
int FloatParameterController<T>::reusableCellCount(int type) {
  if (type == k_buttonCellType) {
    return 1;
  }
  return reusableParameterCellCount(type);
}

template <typename T>
HighlightCell *FloatParameterController<T>::reusableCell(int index, int type) {
  if (type == k_buttonCellType) {
    return &m_okButton;
  }
  return reusableParameterCell(index, type);
}

template <typename T>
void FloatParameterController<T>::willDisplayCellForIndex(HighlightCell *cell,
                                                          int index) {
  if (typeAtIndex(index) == k_buttonCellType ||
      textFieldOfCellAtIndex(cell, index)->isEditing()) {
    return;
  }
  constexpr int precision = Preferences::VeryLargeNumberOfSignificantDigits;
  constexpr int bufferSize =
      PrintFloat::charSizeForFloatsWithPrecision(precision);
  char buffer[bufferSize];
  PoincareHelpers::ConvertFloatToTextWithDisplayMode<T>(
      parameterAtIndex(index), buffer, bufferSize, precision,
      Preferences::PrintFloatMode::Decimal);
  textFieldOfCellAtIndex(cell, index)->setText(buffer);
}

template <typename T>
KDCoordinate FloatParameterController<T>::nonMemoizedRowHeight(int j) {
  if (typeAtIndex(j) == k_buttonCellType) {
    return m_okButton.minimalSizeForOptimalDisplay().height();
  }
  return SelectableListViewController::nonMemoizedRowHeight(j);
}

template <typename T>
bool FloatParameterController<T>::textFieldShouldFinishEditing(
    AbstractTextField *textField, Ion::Events::Event event) {
  return (event == Ion::Events::Down && selectedRow() < numberOfRows() - 1) ||
         (event == Ion::Events::Up && selectedRow() > 0) ||
         TextFieldDelegate::textFieldShouldFinishEditing(textField, event);
}

template <typename T>
bool FloatParameterController<T>::textFieldDidFinishEditing(
    AbstractTextField *textField, const char *text, Ion::Events::Event event) {
  T floatBody =
      textFieldDelegateApp()->template parseInputtedFloatValue<T>(text);
  int row = selectedRow();
  if (hasUndefinedValue(text, floatBody) ||
      !setParameterAtIndex(row, floatBody)) {
    return false;
  }
  resetMemoization();
  m_selectableListView.reloadCell(activeCell());
  m_selectableListView.reloadData();
  if (event == Ion::Events::EXE || event == Ion::Events::OK) {
    m_selectableListView.selectCell(row + 1);
  } else {
    m_selectableListView.handleEvent(event);
  }
  return true;
}

template <typename T>
void FloatParameterController<T>::buttonAction() {
  StackViewController *stack = FloatParameterController<T>::stackController();
  stack->pop();
}

template <typename T>
bool FloatParameterController<T>::hasUndefinedValue(const char *text,
                                                    T floatValue) const {
  InfinityTolerance infTolerance = infinityAllowanceForRow(selectedRow());
  return textFieldDelegateApp()->hasUndefinedValue(
      floatValue, infTolerance == InfinityTolerance::PlusInfinity,
      infTolerance == InfinityTolerance::MinusInfinity);
}

template class FloatParameterController<float>;
template class FloatParameterController<double>;

}  // namespace Shared
