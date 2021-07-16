#include "input_goodness_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <escher/container.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/invocation.h>
#include <escher/responder.h>
#include <escher/selectable_table_view_data_source.h>
#include <escher/table_view_data_source.h>
#include <escher/text_field_delegate.h>

#include "probability/app.h"
#include "probability/models/data.h"
#include "probability/text_helpers.h"

using namespace Probability;

InputGoodnessController::InputGoodnessController(
    StackViewController * parent,
    ResultsController * resultsController,
    GoodnessStatistic * statistic,
    InputEventHandlerDelegate * inputEventHandlerDelegate) :
    InputCategoricalController(parent, resultsController, statistic, inputEventHandlerDelegate),
    m_inputTableView(&m_contentView, inputEventHandlerDelegate, statistic, &m_tableController, &m_contentView, this),
    m_tableController(this, statistic, &m_inputTableView) {
  m_contentView.setTableView(&m_tableController);
}

// ScrollViewDelegate
void Probability::InputGoodnessController::tableViewDidChangeSelectionAndDidScroll(
    SelectableTableView * t,
    int previousSelectedCellX,
    int previousSelectedCellY,
    bool withinTemporarySelection) {
  int row = m_inputTableView.selectedRow();
  int col = m_inputTableView.selectedColumn();
  if (!withinTemporarySelection && previousSelectedCellY != row) {
    // Make m_contentView to cell
    KDRect cellFrame = KDRect(
        m_inputTableView.cumulatedWidthFromIndex(col),
        m_inputTableView.cumulatedHeightFromIndex(row),
        m_inputTableView.columnWidth(col),
        m_inputTableView.rowHeight(row));  // TODO query m_inputTableView::cellFrame

    m_contentView.scrollToContentRect(cellFrame);

    m_contentView.layoutSubviews(true);
  }
}
