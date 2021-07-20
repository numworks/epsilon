#include "input_homogeneity_controller.h"

#include <apps/i18n.h>
#include <escher/container.h>
#include <escher/palette.h>
#include <kandinsky/color.h>
#include <kandinsky/font.h>
#include <string.h>

#include "probability/app.h"
#include "probability/models/data.h"
#include "probability/text_helpers.h"

using namespace Probability;

InputHomogeneityController::InputHomogeneityController(
    StackViewController * parent,
    ResultsHomogeneityController * homogeneityResultsController,
    InputEventHandlerDelegate * inputEventHandlerDelegate,
    HomogeneityStatistic * statistic) :
    InputCategoricalController(parent,
                               homogeneityResultsController,
                               statistic,
                               inputEventHandlerDelegate),
    ChainedSelectableTableViewDelegate(&m_tableData),
    m_innerTableData(&m_table,
                     inputEventHandlerDelegate,
                     statistic,
                     &m_tableController,
                     &m_contentView),
    m_tableData(&m_innerTableData),
    m_table(&m_tableController, &m_tableData, m_contentView.selectionDataSource(), this),
    m_tableController(&m_contentView, &m_table, &m_innerTableData, statistic) {
  m_contentView.setTableView(&m_tableController);
}

void Probability::InputHomogeneityController::didBecomeFirstResponder() {
  App::app()->setPage(Data::Page::InputHomogeneity);
  InputCategoricalController::didBecomeFirstResponder();
}

void Probability::InputHomogeneityController::tableViewDidChangeSelectionAndDidScroll(
    SelectableTableView * t,
    int previousSelectedCellX,
    int previousSelectedCellY,
    bool withinTemporarySelection) {
  ChainedSelectableTableViewDelegate::tableViewDidChangeSelectionAndDidScroll(
      t,
      previousSelectedCellX,
      previousSelectedCellY,
      withinTemporarySelection);
  // TODO factor with InputGoodnessController
  int row = m_table.selectedRow();
  int col = m_table.selectedColumn();
  if (!withinTemporarySelection && previousSelectedCellY != row) {
    // Scroll to cell
    KDRect cellFrame = KDRect(
        0,
        m_tableData.cumulatedHeightFromIndex(row),
        m_tableData.columnWidth(col),
        m_tableData.rowHeight(row));  // TODO query m_inputTableView::cellFrame

    m_contentView.scrollToContentRect(cellFrame);
  }
}
