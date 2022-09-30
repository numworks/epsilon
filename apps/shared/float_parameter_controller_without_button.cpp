#include "float_parameter_controller_without_button.h"
#include "poincare_helpers.h"
#include <escher/buffer_table_cell_with_editable_text.h>
#include <poincare/preferences.h>
#include <assert.h>
#include <cmath>

using namespace Escher;
using namespace Poincare;

namespace Shared {

template<typename T>
FloatParameterControllerWithoutButton<T>::FloatParameterControllerWithoutButton(Responder * parentResponder) :
  SelectableListViewController(parentResponder)
{}

template<typename T>
void FloatParameterControllerWithoutButton<T>::didBecomeFirstResponder() {
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
void FloatParameterControllerWithoutButton<T>::viewWillAppear() {
  ViewController::viewWillAppear();
  int selRow = selectedRow();
  if (selRow == -1) {
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
void FloatParameterControllerWithoutButton<T>::viewDidDisappear() {
  if (parentResponder() == nullptr) {
    m_selectableTableView.deselectTable();
    m_selectableTableView.scrollToCell(0,0);
  }
}

template<typename T>
bool FloatParameterControllerWithoutButton<T>::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Back) {
    stackController()->pop();
    return true;
  }
  return false;
}

template<typename T>
int FloatParameterControllerWithoutButton<T>::typeAtIndex(int index) {
  return k_parameterCellType;
}

template<typename T>
void FloatParameterControllerWithoutButton<T>::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (isCellEditing(cell, index)) {
    return;
  }
  constexpr int precision = Preferences::VeryLargeNumberOfSignificantDigits;
  constexpr int bufferSize = PrintFloat::charSizeForFloatsWithPrecision(precision);
  char buffer[bufferSize];
  PoincareHelpers::ConvertFloatToTextWithDisplayMode<T>(parameterAtIndex(index), buffer, bufferSize, precision, Preferences::PrintFloatMode::Decimal);
  setTextInCell(cell, buffer, index);
}

template<typename T>
KDCoordinate FloatParameterControllerWithoutButton<T>::nonMemoizedRowHeight(int j) {
  return SelectableListViewController::nonMemoizedRowHeight(j);
}

template<typename T>
bool FloatParameterControllerWithoutButton<T>::textFieldShouldFinishEditing(AbstractTextField * textField, Ion::Events::Event event) {
  return (event == Ion::Events::Down && selectedRow() < numberOfRows()-1)
      || (event == Ion::Events::Up && selectedRow() > 0)
      || TextFieldDelegate::textFieldShouldFinishEditing(textField, event);
}

template<typename T>
bool FloatParameterControllerWithoutButton<T>::textFieldDidFinishEditing(AbstractTextField * textField, const char * text, Ion::Events::Event event) {
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
bool FloatParameterControllerWithoutButton<T>::isCellEditing(Escher::HighlightCell * cell, int index) {
  return static_cast<MessageTableCellWithEditableText *>(cell)->isEditing();
}

template<typename T>
void FloatParameterControllerWithoutButton<T>::setTextInCell(Escher::HighlightCell * cell, const char * text, int index) {
  static_cast<MessageTableCellWithEditableText *>(cell)->setAccessoryText(text);
}

template class FloatParameterControllerWithoutButton<float>;
template class FloatParameterControllerWithoutButton<double>;

}
