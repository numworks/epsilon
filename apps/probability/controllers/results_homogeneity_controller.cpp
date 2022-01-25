#include "results_homogeneity_controller.h"

#include <apps/i18n.h>
#include <escher/container.h>
#include <escher/palette.h>
#include <kandinsky/font.h>

#include "probability/app.h"

using namespace Probability;


ResultsHomogeneityController::ResultsHomogeneityController(
    StackViewController * stackViewController,
    HomogeneityStatistic * statistic,
    ResultsController * resultsController) :
      Escher::ViewController(stackViewController),
      m_resultsController(resultsController),
      m_contentView(this, &m_table, this),
      m_tableData(statistic, this, this),
      m_table(this, &m_tableData, &m_tableData, &m_tableData),
      m_isTableSelected(true) {
  m_table.setBackgroundColor(Escher::Palette::WallScreenDark);
  m_table.setDecoratorType(Escher::ScrollView::Decorator::Type::None);
  m_table.setMargins(ResultsHomogeneityView::k_topMargin,
                     Metric::CommonRightMargin,
                     ResultsHomogeneityView::k_topMargin,
                     Metric::CommonLeftMargin);
  m_tableData.setScrollViewDelegate(&m_table);
}

void ResultsHomogeneityController::didBecomeFirstResponder() {
  Probability::App::app()->setPage(Data::Page::ResultsHomogeneity);
  Escher::Container::activeApp()->setFirstResponder(&m_table);
  if (m_table.selectedRow() < 0) {
    m_table.selectCellAtLocation(1, 1);
  }
  m_table.reloadData(false);
  selectCorrectView();
  m_contentView.reload();
}

bool Probability::ResultsHomogeneityController::handleEvent(Ion::Events::Event event) {
  // Handle selection between table and button
  if ((event == Ion::Events::Up && !m_isTableSelected) ||
      (event == Ion::Events::Down && m_isTableSelected)) {
    m_isTableSelected = event == Ion::Events::Up;
    selectCorrectView();
    return true;
  }
  return false;
}

bool Probability::ResultsHomogeneityController::buttonAction() {
  stackOpenPage(m_resultsController, 1);
  return true;
}

void Probability::ResultsHomogeneityController::tableViewDidChangeSelectionAndDidScroll(
    SelectableTableView * t,
    int previousSelectedCellX,
    int previousSelectedCellY,
    bool withinTemporarySelection) {
  scrollToCorrectLocation();
}

void Probability::ResultsHomogeneityController::initCell(EvenOddBufferTextCell, void * cell, int index) {
  static_cast<EvenOddBufferTextCell *>(cell)->setFont(KDFont::SmallFont);
}

void Probability::ResultsHomogeneityController::selectCorrectView() {
  m_contentView.button()->setHighlighted(!m_isTableSelected);
  Escher::Responder * responder;
  if (m_isTableSelected) {
    responder = &m_table;
  } else {
    responder = m_contentView.button();
  }
  Container::activeApp()->setFirstResponder(responder);
  scrollToCorrectLocation();
}

void Probability::ResultsHomogeneityController::scrollToCorrectLocation() {
  if (m_isTableSelected) {
    // Scroll to cell
    int row = m_table.selectedRow(), column = m_table.selectedColumn();
    KDRect cellRect = KDRect(0,
                               m_tableData.cumulatedHeightFromIndex(row) + m_table.topMargin(),
                               m_tableData.columnWidth(column),
                               m_tableData.rowHeight(row));
    cellRect = cellRect.translatedBy(m_contentView.tableOrigin());
    /* Include the title in the first row cells to force scrolling enough to
     * display it */
    if (row == 0) {
      cellRect = cellRect.unionedWith(KDRect(cellRect.x(), 0, cellRect.width(), cellRect.height()));
    }
    m_contentView.scrollToContentRect(cellRect);
  } else {
    // Scroll to button
    m_contentView.scrollToContentRect(
        KDRect(m_contentView.buttonOrigin(), m_contentView.button()->bounds().size()));
  }
}
