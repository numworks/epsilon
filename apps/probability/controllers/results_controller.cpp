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
                                     TestGraphController * testGraphController,
                                     IntervalGraphController * intervalGraphController,
                                     Escher::InputEventHandlerDelegate * handler,
                                     Escher::TextFieldDelegate * textFieldDelegate) :
      Escher::ViewController(parent),
      m_tableView(this, &m_resultsDataSource, this, &m_contentView),
      m_title(KDFont::SmallFont, I18n::Message::CalculatedValues, KDContext::k_alignCenter, KDContext::k_alignCenter, Palette::GrayDark, Palette::WallScreen),
      m_contentView(&m_tableView, &m_resultsDataSource, &m_title),
      m_resultsDataSource(&m_tableView, statistic, Escher::Invocation(&ResultsController::ButtonAction, this), this),
      m_statistic(statistic),
      m_testGraphController(testGraphController),
      m_intervalGraphController(intervalGraphController) {
}

void ResultsController::didBecomeFirstResponder() {
  selectCellAtLocation(0, 0);
  Escher::Container::activeApp()->setFirstResponder(&m_tableView);
  m_resultsDataSource.resetMemoization();
  m_contentView.reload();
}

ViewController::TitlesDisplay Probability::ResultsController::titlesDisplay() {
  if (m_statistic->subApp() == Inference::SubApp::Interval || (m_statistic->significanceTestType() == SignificanceTestType::Categorical && m_statistic->categoricalType() == CategoricalType::GoodnessOfFit)) {
    return ViewController::TitlesDisplay::DisplayLastTwoTitles;
  }
  return ViewController::TitlesDisplay::DisplayLastThreeTitles;
}

const char * Probability::ResultsController::title() {
  m_titleBuffer[0] = 0;
  StackViewController * stackViewControllerResponder = static_cast<StackViewController *>(parentResponder());
  m_statistic->setResultTitle(m_titleBuffer, sizeof(m_titleBuffer), stackViewControllerResponder->topViewController() != this);
  if (m_titleBuffer[0] == 0) {
    return nullptr;
  }
  return m_titleBuffer;
}

bool Probability::ResultsController::ButtonAction(void * c, void * s) {
  ResultsController * controller = static_cast<ResultsController *>(c);
  if (!controller->m_statistic->isGraphable()) {
    App::app()->displayWarning(I18n::Message::InvalidValues);
    return false;
  }
  if (ExamModeConfiguration::testsGraphResultsAreForbidden() && controller->m_statistic->subApp() == Inference::SubApp::Test) {
    App::app()->displayWarning(I18n::Message::DisabledFeatureInTestMode1, I18n::Message::DisabledFeatureInTestMode2);
    return false;
  }
  Escher::ViewController * graph;
  if (controller->m_statistic->subApp() == Inference::SubApp::Test) {
    graph = controller->m_testGraphController;
  } else {
    graph = controller->m_intervalGraphController;
  }
  controller->stackOpenPage(graph);
  return true;
}

void Probability::ResultsController::initCell(ExpressionCellWithBufferWithMessage, void * cell, int index) {
  static_cast<ExpressionCellWithBufferWithMessage *>(cell)->setParentResponder(&m_tableView);
}
