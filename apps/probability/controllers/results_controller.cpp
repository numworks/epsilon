#include "results_controller.h"

#include <apps/i18n.h>
#include <apps/exam_mode_configuration.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/invocation.h>
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
      Escher::ViewController(parent),
      m_tableView(this, &m_resultsDataSource, this, &m_contentView),
      m_title(KDFont::SmallFont, I18n::Message::CalculatedValues, KDContext::k_alignCenter, KDContext::k_alignCenter, Palette::GrayDark, Palette::WallScreen),
      m_contentView(&m_tableView, &m_resultsDataSource, &m_title),
      m_resultsDataSource(&m_tableView, statistic, Escher::Invocation(&ResultsController::ButtonAction, this), this),
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

bool Probability::ResultsController::ButtonAction(void * c, void * s) {
  ResultsController * controller = static_cast<ResultsController *>(s);
  if (!controller->m_statistic->isGraphable()) {
    App::app()->displayWarning(I18n::Message::InvalidValues);
    return false;
  }
  if (ExamModeConfiguration::testsGraphResultsAreForbidden() && App::app()->subapp() == Data::SubApp::Tests) {
    App::app()->displayWarning(I18n::Message::DisabledFeatureInTestMode1, I18n::Message::DisabledFeatureInTestMode2);
    return false;
  }
  controller->stackOpenPage(controller->m_statisticGraphController, (App::app()->test() == Data::Test::Categorical ? 1 : 2));
  return true;
}

void Probability::ResultsController::initCell(ExpressionCellWithBufferWithMessage, void * cell, int index) {
  static_cast<ExpressionCellWithBufferWithMessage *>(cell)->setParentResponder(&m_tableView);
}
