#include "explicit_float_parameter_controller.h"
#include "poincare_helpers.h"
#include "text_field_delegate_app.h"
#include <escher/buffer_table_cell_with_editable_text.h>
#include <poincare/preferences.h>
#include <assert.h>
#include <cmath>

using namespace Escher;
using namespace Poincare;

namespace Shared {

ExplicitFloatParameterController::ExplicitFloatParameterController(Responder * parentResponder) :
  ExplicitSelectableListViewController(parentResponder)
{}

void ExplicitFloatParameterController::didBecomeFirstResponder() {
  if (selectedRow() >= 0) {
    int selRow = std::min(selectedRow(), numberOfRows() - 1);
    int selColumn = std::min(selectedColumn(), numberOfColumns() - 1);
    selectCellAtLocation(selColumn, selRow);
  }
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

void ExplicitFloatParameterController::viewWillAppear() {
  ViewController::viewWillAppear();
  int selRow = selectedRow();
  if (selRow == -1) {
    selectCellAtLocation(0, 0);
  } else {
    selRow = std::min(selectedRow(), numberOfRows() - 1);
    int selColumn = std::min(selectedColumn(), numberOfColumns() - 1);
    selectCellAtLocation(selColumn, selRow);
  }
  resetMemoization();
  m_selectableTableView.reloadData();
}

void ExplicitFloatParameterController::viewDidDisappear() {
  if (parentResponder() == nullptr) {
    m_selectableTableView.deselectTable();
    m_selectableTableView.scrollToCell(0,0);
  }
}

bool ExplicitFloatParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Back) {
    stackController()->pop();
    return true;
  }
  return false;
}

void ExplicitFloatParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (isCellEditing(cell, index)) {
    return;
  }
  constexpr int precision = Preferences::VeryLargeNumberOfSignificantDigits;
  constexpr int bufferSize = PrintFloat::charSizeForFloatsWithPrecision(precision);
  char buffer[bufferSize];
  PoincareHelpers::ConvertFloatToTextWithDisplayMode(parameterAtIndex(index), buffer, bufferSize, precision, Preferences::PrintFloatMode::Decimal);
  setTextInCell(cell, buffer, index);
}

KDCoordinate ExplicitFloatParameterController::nonMemoizedRowHeight(int j) {
  return SelectableListViewController::nonMemoizedRowHeight(j);
}

bool ExplicitFloatParameterController::textFieldShouldFinishEditing(AbstractTextField * textField, Ion::Events::Event event) {
  return (event == Ion::Events::Down && selectedRow() < numberOfRows()-1)
      || (event == Ion::Events::Up && selectedRow() > 0)
      || TextFieldDelegate::textFieldShouldFinishEditing(textField, event);
}

bool ExplicitFloatParameterController::textFieldDidFinishEditing(AbstractTextField * textField, const char * text, Ion::Events::Event event) {
  float floatBody;
  int row = selectedRow();
  if (textFieldDelegateApp()->hasUndefinedValue(text, &floatBody)) {
    return false;
  }
  if (!setParameterAtIndex(row, floatBody)) {
    return false;
  }
  resetMemoization();
  m_selectableTableView.reloadCellAtLocation(0, selectedRow());
  m_selectableTableView.reloadData();
  if (event == Ion::Events::EXE || event == Ion::Events::OK) {
    m_selectableTableView.selectCellAtLocation(selectedColumn(), m_selectableTableView.indexOfNextSelectableRow(1));
  } else {
    m_selectableTableView.handleEvent(event);
  }
  return true;
}


bool ExplicitFloatParameterController::isCellEditing(Escher::HighlightCell * cell, int index) {
  return static_cast<BufferTableCellWithEditableText *>(cell)->isEditing();
}

void ExplicitFloatParameterController::setTextInCell(Escher::HighlightCell * cell, const char * text, int index) {
  static_cast<BufferTableCellWithEditableText *>(cell)->setAccessoryText(text);
}

}
