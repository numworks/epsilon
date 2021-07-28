#include "results_controller.h"

#include <apps/i18n.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/responder.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/text_field_delegate.h>

#include "probability/app.h"
#include "probability/helpers.h"
#include "probability/models/data.h"
#include "probability/text_helpers.h"

using namespace Probability;

ResultsController::ResultsController(Escher::StackViewController * parent,
                                     Statistic * statistic,
                                     StatisticGraphController * statisticGraphController,
                                     Escher::InputEventHandlerDelegate * handler,
                                     Escher::TextFieldDelegate * textFieldDelegate) :
    Page(parent),
    m_tableView(this, &m_resultsDataSource, this, &m_contentView),
    m_contentView(&m_tableView, &m_resultsDataSource),
    m_resultsDataSource(&m_tableView, statistic, this),
    m_statistic(statistic),
    m_statisticGraphController(statisticGraphController) {
}

void ResultsController::didBecomeFirstResponder() {
  Probability::App::app()->setPage(Data::Page::Results);
  // TODO factor out
  if (selectedRow() == -1) {
    selectCellAtLocation(0, 0);
  } else {
    selectCellAtLocation(selectedColumn(),
                         clipped(selectedRow(), m_resultsDataSource.numberOfRows() - 1));
  }
  Escher::Container::activeApp()->setFirstResponder(&m_tableView);
  m_resultsDataSource.resetMemoization();
  m_contentView.reload();
}

ViewController::TitlesDisplay Probability::ResultsController::titlesDisplay() {
  if (App::app()->categoricalType() == Data::CategoricalType::None) {
    return ViewController::TitlesDisplay::DisplayLastTwoTitles;
  }
  return ViewController::TitlesDisplay::DisplayLastTitle;
}

const char * Probability::ResultsController::title() {
  if (App::app()->subapp() == Data::SubApp::Intervals) {
    char confidenceBuffer[k_titleBufferSize];
    defaultParseFloat(m_statistic->threshold(), confidenceBuffer, k_titleBufferSize);
    const char * estimateSymbol = m_statistic->estimateSymbol();
    char estimateBuffer[k_titleBufferSize];
    defaultParseFloat(m_statistic->estimate(), estimateBuffer, k_titleBufferSize);

    snprintf(m_titleBuffer,
             sizeof(m_titleBuffer),
             "%s=%s Confidence=%s",
             estimateSymbol,
             estimateBuffer,
             confidenceBuffer);
    return m_titleBuffer;
  }
  return nullptr;
}

bool Probability::ResultsController::buttonAction() {
  openPage(m_statisticGraphController);
  return true;
}

Probability::ResultsView::ContentView::ContentView(Escher::SelectableTableView * table,
                                                   I18n::Message titleMessage) :
    m_spacer(Palette::WallScreen, 0, k_spacerHeight),
    m_title(KDFont::SmallFont, titleMessage, 0.5f, 0.5f, Palette::GrayDark, Palette::WallScreen),
    m_table(table) {
  m_table->setMargins(k_spacerHeight, Metric::CommonRightMargin, 0, Metric::CommonLeftMargin);
}

Escher::View * Probability::ResultsView::ContentView::subviewAtIndex(int i) {
  assert(i < numberOfSubviews());
  if (i == 0) {
    return &m_spacer;
  } else if (i == 1) {
    return &m_title;
  }
  return m_table;
}

void Probability::ResultsView::ContentView::relayout() {
  layoutSubviews();
}

Probability::ResultsView::ResultsView(Escher::SelectableTableView * table,
                                      Escher::TableViewDataSource * tableDataSource,
                                      I18n::Message titleMessage) :
    ScrollView(&m_contentView, &m_scrollDataSource),
    m_contentView(table, titleMessage),
    m_tableDataSource(tableDataSource) {
  setMargins(ContentView::k_spacerHeight, 0, Metric::CommonBottomMargin, 0);
}

KDPoint Probability::ResultsView::tableOrigin() {
  return KDPoint(0, 2 * ContentView::k_spacerHeight + 14);
}

void Probability::ResultsView::tableViewDidChangeSelectionAndDidScroll(
    SelectableTableView * t,
    int previousSelectedCellX,
    int previousSelectedCellY,
    bool withinTemporarySelection) {
  // Scroll to correct location
  int row = m_contentView.m_table->selectedRow(), col = m_contentView.m_table->selectedColumn();
  if (row >= 0 && col >= 0) {
    KDRect cellFrame = KDRect(m_tableDataSource->cumulatedWidthFromIndex(col),
                              m_tableDataSource->cumulatedHeightFromIndex(row),
                              m_tableDataSource->columnWidth(col),
                              m_tableDataSource->rowHeight(row));
    scrollToContentRect(cellFrame.translatedBy(tableOrigin()));
  }
}

void Probability::ResultsView::reload() {
  m_contentView.m_table->reloadData();
  layoutSubviews();
}
