#include "column_parameter_controller.h"
#include "editable_cell_table_view_controller.h"
#include <assert.h>
#include <escher/message_table_cell_with_editable_text.h>
#include <poincare/print.h>

using namespace Escher;

namespace Shared {

ColumnParameterController::ColumnParameterController(Responder * parentResponder) :
  SelectableListViewController(parentResponder),
  m_columnIndex(-1)
{ }

void ColumnParameterController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

void ColumnParameterController::viewWillAppear() {
  resetMemoization();
  m_selectableTableView.reloadData();
}

const char * ColumnParameterController::title() {
  return m_titleBuffer;
}

void ColumnParameterController::initializeColumnParameters() {
  m_columnIndex = editableCellTableViewController()->selectedColumn();
  editableCellTableViewController()->fillColumnName(m_columnIndex, m_columnNameBuffer);
  Poincare::Print::customPrintf(m_titleBuffer, sizeof(m_titleBuffer),
      I18n::translate(I18n::Message::ColumnOptions), m_columnNameBuffer);
}

Escher::StackViewController * ColumnParameterController::stackView() {
 return static_cast<Escher::StackViewController *>(parentResponder());
}

}
