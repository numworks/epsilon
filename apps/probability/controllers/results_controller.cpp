#include "results_controller.h"

#include <apps/i18n.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/responder.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/text_field_delegate.h>

#include <algorithm>

#include "probability/app.h"
#include "probability/helpers.h"
#include "probability/models/data.h"
#include "probability/text_helpers.h"

using namespace Probability;

KDColor ResultsController::s_titleColor = KDColor::RGB24(0xBEBBC6);

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
  selectCellAtLocation(0, 0);
  Escher::Container::activeApp()->setFirstResponder(&m_tableView);
  m_resultsDataSource.resetMemoization();
  m_contentView.reload();
}

ViewController::TitlesDisplay Probability::ResultsController::titlesDisplay() {
  if (App::app()->categoricalType() == Data::CategoricalType::Unset) {
    return ViewController::TitlesDisplay::DisplayLastTwoTitles;
  }
  return ViewController::TitlesDisplay::DisplayLastTitle;
}

const char * Probability::ResultsController::title() {
  if (App::app()->subapp() == Data::SubApp::Intervals) {
    char confidenceBuffer[k_titleBufferSize];
    defaultParseFloat(m_statistic->threshold(), confidenceBuffer, k_titleBufferSize);
    const char * confidence = I18n::translate(I18n::Message::Confidence);
    if (App::app()->page() == Data::Page::Graph) {
      const char * estimateSymbol = m_statistic->estimateSymbol();
      char estimateBuffer[k_titleBufferSize];
      defaultParseFloat(m_statistic->estimate(), estimateBuffer, k_titleBufferSize);
      snprintf(m_titleBuffer,
               sizeof(m_titleBuffer),
               "%s=%s %s=%s",
               estimateSymbol,
               estimateBuffer,
               confidence,
               confidenceBuffer);
    } else {
      snprintf(m_titleBuffer, sizeof(m_titleBuffer), "%s=%s", confidence, confidenceBuffer);
    }
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
    m_title(KDFont::SmallFont, titleMessage, 0.5f, 0.5f, Palette::GrayDark, Palette::WallScreen),
    m_table(table) {
  m_table->setMargins(k_marginAroundTitle, Metric::CommonRightMargin, 0, Metric::CommonLeftMargin);
}

Escher::View * Probability::ResultsView::ContentView::subviewAtIndex(int i) {
  assert(i < numberOfSubviews());
  if (i == 0) {
    return &m_title;
  } else {
    return m_table;
  }
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
  setMargins(ContentView::k_marginAroundTitle, 0, Metric::CommonBottomMargin, 0);
}

KDPoint Probability::ResultsView::tableOrigin() {
  return KDPoint(0, ContentView::k_marginAroundTitle + KDFont::SmallFont->glyphSize().height());
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
