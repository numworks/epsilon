#include "float_parameter_controller.h"
#include "poincare_helpers.h"
#include "text_field_delegate_app.h"
#include <escher/message_table_cell_with_editable_text.h>
#include <poincare/preferences.h>
#include <assert.h>
#include <cmath>

using namespace Escher;
using namespace Poincare;

namespace Shared {

template<typename T>
FloatParameterController<T>::FloatParameterController(Responder * parentResponder) :
  SelectableListViewController(parentResponder),
  m_okButton(&(this->m_selectableTableView), I18n::Message::Ok, Invocation::Builder<FloatParameterController>([](FloatParameterController * parameterController, void * sender) {
      parameterController->buttonAction();
      return true;
    }, this))
{}

template<typename T>
void FloatParameterController<T>::didBecomeFirstResponder() {
  if (selectedRow() >= 0) {
    int selRow = selectedRow();
    selRow = selRow >= numberOfRows() ? numberOfRows()-1 : selRow;
    int selColumn = selectedColumn();
    selColumn = selColumn >= numberOfColumns() ? numberOfColumns() - 1 : selColumn;
    selectCellAtLocation(selColumn, selRow);
  }
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

template<typename T>
void FloatParameterController<T>::viewWillAppear() {
  ViewController::viewWillAppear();
  int selRow = selectedRow();
  if (selRow == -1 || typeAtIndex(selRow) == k_buttonCellType) {
    selectCellAtLocation(0, 0);
  } else {
    selRow = selRow >= numberOfRows() ? numberOfRows()-1 : selRow;
    int selColumn = selectedColumn();
    selColumn = selColumn >= numberOfColumns() ? numberOfColumns() - 1 : selColumn;
    selectCellAtLocation(selColumn, selRow);
  }
  resetMemoization();
  m_selectableTableView.reloadData();
}

template<typename T>
void FloatParameterController<T>::viewDidDisappear() {
  if (parentResponder() == nullptr) {
    m_selectableTableView.deselectTable();
    m_selectableTableView.scrollToCell(0,0);
  }
}

template<typename T>
bool FloatParameterController<T>::handleEvent(Ion::Events::Event & event) {
  if (event == Ion::Events::Back) {
    stackController()->pop();
    return true;
  }
  return false;
}

template<typename T>
int FloatParameterController<T>::typeAtIndex(int index) const {
  if (index == this->numberOfRows() - 1) {
    return k_buttonCellType;
  }
  return k_parameterCellType;
}

template<typename T>
int FloatParameterController<T>::reusableCellCount(int type) {
  if (type == k_buttonCellType) {
    return 1;
  }
  return reusableParameterCellCount(type);
}

template<typename T>
HighlightCell * FloatParameterController<T>::reusableCell(int index, int type) {
  if (type == k_buttonCellType) {
    return &m_okButton;
  }
  return reusableParameterCell(index, type);
}

template<typename T>
void FloatParameterController<T>::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (typeAtIndex(index) == k_buttonCellType || isCellEditing(cell, index)) {
    return;
  }
  constexpr int precision = Preferences::VeryLargeNumberOfSignificantDigits;
  constexpr int bufferSize = PrintFloat::charSizeForFloatsWithPrecision(precision);
  char buffer[bufferSize];
  PoincareHelpers::ConvertFloatToTextWithDisplayMode<T>(parameterAtIndex(index), buffer, bufferSize, precision, Preferences::PrintFloatMode::Decimal);
  setTextInCell(cell, buffer, index);
}

template<typename T>
KDCoordinate FloatParameterController<T>::nonMemoizedRowHeight(int j) {
  if (typeAtIndex(j) == k_buttonCellType) {
    return m_okButton.minimalSizeForOptimalDisplay().height();
  }
  return SelectableListViewController::nonMemoizedRowHeight(j);
}

template<typename T>
bool FloatParameterController<T>::textFieldShouldFinishEditing(AbstractTextField * textField, Ion::Events::Event event) {
  return (event == Ion::Events::Down && selectedRow() < numberOfRows()-1)
      || (event == Ion::Events::Up && selectedRow() > 0)
      || TextFieldDelegate::textFieldShouldFinishEditing(textField, event);
}

template<typename T>
bool FloatParameterController<T>::textFieldDidFinishEditing(AbstractTextField * textField, const char * text, Ion::Events::Event event) {
  T floatBody;
  int row = selectedRow();
  InfinityTolerance infTolerance = infinityAllowanceForRow(row);
  if (textFieldDelegateApp()->hasUndefinedValue(text, &floatBody, infTolerance == InfinityTolerance::PlusInfinity, infTolerance == InfinityTolerance::MinusInfinity)) {
    return false;
  }
  if (!setParameterAtIndex(row, floatBody)) {
    return false;
  }
  resetMemoization();
  m_selectableTableView.reloadCellAtLocation(0, activeCell());
  m_selectableTableView.reloadData();
  if (event == Ion::Events::EXE || event == Ion::Events::OK) {
    m_selectableTableView.selectCellAtLocation(selectedColumn(), row + 1);
  } else {
    m_selectableTableView.handleEvent(event);
  }
  return true;
}


template<typename T>
bool FloatParameterController<T>::isCellEditing(Escher::HighlightCell * cell, int index) {
  return static_cast<MessageTableCellWithEditableText *>(cell)->isEditing();
}

template<typename T>
void FloatParameterController<T>::setTextInCell(Escher::HighlightCell * cell, const char * text, int index) {
  static_cast<MessageTableCellWithEditableText *>(cell)->setAccessoryText(text);
}

template<typename T>
void FloatParameterController<T>::buttonAction() {
  StackViewController * stack = FloatParameterController<T>::stackController();
  stack->pop();
}

template class FloatParameterController<float>;
template class FloatParameterController<double>;

}
