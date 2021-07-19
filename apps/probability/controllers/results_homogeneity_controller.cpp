#include "results_homogeneity_controller.h"

#include <apps/i18n.h>
#include <escher/container.h>
#include <escher/palette.h>
#include <kandinsky/font.h>

#include "probability/app.h"
#include "probability/models/data.h"

using namespace Probability;

HomogeneityResultsView::ContentView::ContentView(Responder * parent,
                                                 SelectableTableView * table,
                                                 ButtonDelegate * buttonDelegate) :
    VerticalLayout(Escher::Palette::WallScreenDark),
    m_topSpacer(Escher::Palette::WallScreenDark, 0, k_topMargin),
    m_title(KDFont::SmallFont,
            I18n::Message::HomogeneityResultsTitle,
            0.5f,
            0.5f,
            Escher::Palette::GrayVeryDark,
            Escher::Palette::WallScreenDark),
    m_table(table),
    m_next(parent,
           I18n::Message::Next,
           buttonDelegate->buttonActionInvocation(),
           KDFont::LargeFont),
    m_buttonWrapper(&m_next) {
}

Probability::HomogeneityResultsView::ContentView::ButtonWithHorizontalMargins::
    ButtonWithHorizontalMargins(Escher::Button * button, KDCoordinate margin) :
    VerticalLayout(Palette::WallScreenDark), m_button(button) {
  setMargins(margin, KDCoordinate(0));
}

Escher::View * Probability::HomogeneityResultsView::ContentView::subviewAtIndex(int i) {
  switch (i) {
    case k_indexOfTopSpacer:
      return &m_topSpacer;
    case k_indexOfTitle:
      return &m_title;
    case k_indexOfTable:
      return m_table;
    case k_indexOfButton:
      return &m_buttonWrapper;
  }
  assert(false);
  return nullptr;
}

KDPoint Probability::HomogeneityResultsView::ContentView::tableOrigin() {
  return KDPoint(0, m_title.minimalSizeForOptimalDisplay().height() + k_topMargin);
}

KDPoint Probability::HomogeneityResultsView::ContentView::buttonOrigin() {
  return KDPoint(0, tableOrigin().y() + m_table->minimalSizeForOptimalDisplay().height());
}

Probability::HomogeneityResultsView::HomogeneityResultsView(Responder * parent,
                                                            SelectableTableView * table,
                                                            ButtonDelegate * buttonDelegate) :
    ScrollView(&m_contentView, &m_scrollDataSource), m_contentView(parent, table, buttonDelegate) {
  setMargins(Metric::CellTopMargin, 0, Metric::CommonBottomMargin, 0);
  setBackgroundColor(Palette::WallScreenDark);
}

KDSize Probability::HomogeneityResultsView::minimalSizeForOptimalDisplay() const {
  // Pass expected size to VerticalLayout to propagate to TableCells
  ContentView * contentView = const_cast<ContentView *>(&m_contentView);
  contentView->setSize(KDSize(bounds().width(), 10000));
  KDSize requiredSize = ScrollView::minimalSizeForOptimalDisplay();
  return KDSize(bounds().width(), requiredSize.height());
}

void Probability::HomogeneityResultsView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorOrange);
}

ResultsHomogeneityController::ResultsHomogeneityController(
    StackViewController * stackViewController,
    HomogeneityStatistic * statistic,
    ResultsController * resultsController) :
    Page(stackViewController),
    ChainedSelectableTableViewDelegate(&m_tableData),
    m_resultsController(resultsController),
    m_contentView(this, &m_table, this),
    m_tableData(&m_innerTableData),
    m_innerTableData(statistic),
    m_table(this, &m_tableData, &m_tableData, this),
    m_isTableSelected(true) {
  m_table.setBackgroundColor(Escher::Palette::WallScreenDark);
  m_table.setDecoratorType(Escher::ScrollView::Decorator::Type::None);
  m_table.setMargins(HomogeneityResultsView::k_topMargin,
                     Metric::CommonRightMargin,
                     HomogeneityResultsView::k_topMargin,
                     Metric::CommonLeftMargin);
}

void ResultsHomogeneityController::didBecomeFirstResponder() {
  Probability::App::app()->setPage(Data::Page::ResultsHomogeneity);
  Escher::Container::activeApp()->setFirstResponder(&m_table);
  if (m_table.selectedRow() < 0) {
    m_table.selectCellAtLocation(1, 1);
  }
  m_table.reloadData(false);
  selectCorrectView();
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

void Probability::ResultsHomogeneityController::buttonAction() {
  openPage(m_resultsController);
}

void Probability::ResultsHomogeneityController::tableViewDidChangeSelectionAndDidScroll(
    SelectableTableView * t,
    int previousSelectedCellX,
    int previousSelectedCellY,
    bool withinTemporarySelection) {
  scrollToCorrectLocation();
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
    KDRect cellOffset = KDRect(0,
                               m_tableData.cumulatedHeightFromIndex(row) + m_table.topMargin(),
                               m_tableData.columnWidth(column),
                               m_tableData.rowHeight(row));
    m_contentView.scrollToContentRect(cellOffset.translatedBy(m_contentView.tableOrigin()));
  } else {
    // Scroll to button
    m_contentView.scrollToContentRect(
        KDRect(m_contentView.buttonOrigin(), m_contentView.button()->bounds().size()));
  }
}
