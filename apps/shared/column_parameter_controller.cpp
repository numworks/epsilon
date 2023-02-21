#include "column_parameter_controller.h"

#include <assert.h>
#include <escher/message_table_cell_with_editable_text.h>
#include <poincare/print.h>

#include "editable_cell_table_view_controller.h"

using namespace Escher;

namespace Shared {

void ColumnParameterController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

void ColumnParameterController::viewWillAppear() {
  m_selectableTableView.reloadData();
}

void ColumnParameters::initializeColumnParameters() {
  m_columnIndex = columnNameHelper()->table()->selectedColumn();
  columnNameHelper()->fillColumnName(m_columnIndex, m_columnNameBuffer);
  Poincare::Print::CustomPrintf(m_titleBuffer, sizeof(m_titleBuffer),
                                I18n::translate(I18n::Message::ColumnOptions),
                                m_columnNameBuffer);
}

Escher::StackViewController *ColumnParameterController::stackView() {
  return static_cast<Escher::StackViewController *>(parentResponder());
}

}  // namespace Shared
