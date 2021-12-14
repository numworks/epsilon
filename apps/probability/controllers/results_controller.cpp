#include "results_controller.h"

#include <apps/i18n.h>
#include <apps/exam_mode_configuration.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/stack_view_controller.h>
#include <escher/text_field_delegate.h>
#include <poincare/print.h>

#include "probability/app.h"
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
      m_resultsDataSource(&m_tableView, statistic, this, this),
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
    const char * confidence = I18n::translate(I18n::Message::Confidence);
    if (App::app()->page() == Data::Page::Graph) {
      const char * estimateSymbol = m_statistic->estimateSymbol();
      Poincare::Print::customPrintf(m_titleBuffer, sizeof(m_titleBuffer), "%s=%*.*ed %s=%*.*ed",
          estimateSymbol,
          m_statistic->estimate(), Poincare::Preferences::PrintFloatMode::Decimal, Poincare::Preferences::ShortNumberOfSignificantDigits,
          confidence,
          m_statistic->threshold(), Poincare::Preferences::PrintFloatMode::Decimal, Poincare::Preferences::ShortNumberOfSignificantDigits);
    } else {
      Poincare::Print::customPrintf(m_titleBuffer, sizeof(m_titleBuffer), "%s=%*.*ed",
          confidence,
          m_statistic->threshold(), Poincare::Preferences::PrintFloatMode::Decimal, Poincare::Preferences::ShortNumberOfSignificantDigits);
    }
    return m_titleBuffer;
  }
  return nullptr;
}

bool Probability::ResultsController::buttonAction() {
  if (!m_statistic->isGraphable()) {
    App::app()->displayWarning(I18n::Message::InvalidValues);
    return false;
  }
  if (ExamModeConfiguration::testsGraphResultsAreForbidden() && App::app()->subapp() == Data::SubApp::Tests) {
    App::app()->displayWarning(I18n::Message::DisabledFeatureInTestMode1, I18n::Message::DisabledFeatureInTestMode2);
    return false;
  }
  openPage(m_statisticGraphController);
  return true;
}

void Probability::ResultsController::initCell(ExpressionCellWithBufferWithMessage, void * cell, int index) {
  static_cast<ExpressionCellWithBufferWithMessage *>(cell)->setParentResponder(&m_tableView);
}

Probability::ResultsView::ContentView::ContentView(Escher::SelectableTableView * table,
                                                   I18n::Message titleMessage) :
      m_title(KDFont::SmallFont,
              titleMessage,
              KDContext::k_alignCenter,
              KDContext::k_alignCenter,
              Palette::GrayDark,
              Palette::WallScreen),
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
    cellFrame = cellFrame.translatedBy(tableOrigin());
    /* Include the title in the first row cells to force scrolling enough to
     * display it */
    // TODO: factorize with ResultsHomogenityController
    if (row == 0) {
      cellFrame = cellFrame.unionedWith(KDRect(cellFrame.x(), 0, cellFrame.width(), cellFrame.height()));
    }
    scrollToContentRect(cellFrame);
  }
}

void Probability::ResultsView::reload() {
  m_contentView.m_table->reloadData();
  layoutSubviews();
}

int Probability::ResultsController::stackTitleStyleStep() const {
  return App::app()->test() == Data::Test::Categorical ? 1 : 2;
}
