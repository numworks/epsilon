#include "float_parameter_controller_without_button.h"
#include "poincare_helpers.h"
#include <escher/buffer_table_cell_with_editable_text.h>
#include <poincare/preferences.h>
#include <assert.h>
#include <cmath>

using namespace Escher;
using namespace Poincare;

namespace Shared {

template<typename T, typename M>
FloatParameterControllerWithoutButton<T,M>::FloatParameterControllerWithoutButton(Responder * parentResponder) :
  SelectableListViewController(parentResponder)
{}

template<typename T, typename M>
void FloatParameterControllerWithoutButton<T,M>::didBecomeFirstResponder() {
  if (selectedRow() >= 0) {
    int selRow = selectedRow();
    selRow = selRow >= numberOfRows() ? numberOfRows()-1 : selRow;
    int selColumn = selectedColumn();
    selColumn = selColumn >= numberOfColumns() ? numberOfColumns() - 1 : selColumn;
    selectCellAtLocation(selColumn, selRow);
  }
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

template<typename T, typename M>
void FloatParameterControllerWithoutButton<T,M>::viewWillAppear() {
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

template<typename T, typename M>
void FloatParameterControllerWithoutButton<T,M>::viewDidDisappear() {
  if (parentResponder() == nullptr) {
    m_selectableTableView.deselectTable();
    m_selectableTableView.scrollToCell(0,0);
  }
}

template<typename T, typename M>
bool FloatParameterControllerWithoutButton<T,M>::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Back) {
    stackController()->pop();
    return true;
  }
  return false;
}

template<typename T, typename M>
int FloatParameterControllerWithoutButton<T,M>::typeAtIndex(int index) {
  return k_parameterCellType;
}

template<typename T, typename M>
void FloatParameterControllerWithoutButton<T,M>::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (isCellEditing(cell, index)) {
    return;
  }
  constexpr int precision = Preferences::VeryLargeNumberOfSignificantDigits;
  constexpr int bufferSize = PrintFloat::charSizeForFloatsWithPrecision(precision);
  char buffer[bufferSize];
  PoincareHelpers::ConvertFloatToTextWithDisplayMode<T>(parameterAtIndex(index), buffer, bufferSize, precision, Preferences::PrintFloatMode::Decimal);
  setTextInCell(cell, buffer, index);
}

template<typename T, typename M>
KDCoordinate FloatParameterControllerWithoutButton<T,M>::nonMemoizedRowHeight(int j) {
  return SelectableListViewController::nonMemoizedRowHeight(j);
}

template<typename T, typename M>
bool FloatParameterControllerWithoutButton<T,M>::textFieldShouldFinishEditing(AbstractTextField * textField, Ion::Events::Event event) {
  return (event == Ion::Events::Down && selectedRow() < numberOfRows()-1)
      || (event == Ion::Events::Up && selectedRow() > 0)
      || TextFieldDelegate::textFieldShouldFinishEditing(textField, event);
}

template<typename T, typename M>
bool FloatParameterControllerWithoutButton<T,M>::textFieldDidFinishEditing(AbstractTextField * textField, const char * text, Ion::Events::Event event) {
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


template<typename T, typename M>
bool FloatParameterControllerWithoutButton<T,M>::isCellEditing(Escher::HighlightCell * cell, int index) {
  return static_cast<M *>(cell)->isEditing();
}

template<typename T, typename M>
void FloatParameterControllerWithoutButton<T,M>::setTextInCell(Escher::HighlightCell * cell, const char * text, int index) {
  static_cast<M *>(cell)->setAccessoryText(text);
}

template class FloatParameterControllerWithoutButton<float>;
template class FloatParameterControllerWithoutButton<double>;
template class FloatParameterControllerWithoutButton<float, BufferTableCellWithEditableText>;

}
