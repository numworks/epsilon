#include "results_controller.h"

#include <apps/i18n.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/invocation.h>
#include <escher/stack_view_controller.h>
#include <escher/text_field_delegate.h>
#include <poincare/print.h>

#include "inference/app.h"
#include "inference/text_helpers.h"

using namespace Escher;

namespace Inference {

ResultsController::ResultsController(
    Escher::StackViewController *parent, Statistic *statistic,
    TestGraphController *testGraphController,
    IntervalGraphController *intervalGraphController,
    Escher::InputEventHandlerDelegate *handler,
    Escher::TextFieldDelegate *textFieldDelegate)
    : Escher::ViewController(parent),
      m_selectableListView(this, &m_resultsDataSource, this),
      m_title(I18n::Message::CalculatedValues,
              {{.glyphColor = Palette::GrayDark,
                .backgroundColor = Palette::WallScreen,
                .font = KDFont::Size::Small},
               .horizontalAlignment = KDGlyph::k_alignCenter}),
      m_contentView(&m_selectableListView, &m_resultsDataSource, &m_title),
      m_resultsDataSource(&m_selectableListView, statistic,
                          Invocation::Builder<ResultsController>(
                              &ResultsController::ButtonAction, this),
                          this),
      m_statistic(statistic),
      m_testGraphController(testGraphController),
      m_intervalGraphController(intervalGraphController) {}

void ResultsController::didBecomeFirstResponder() {
  selectCell(0);
  Escher::Container::activeApp()->setFirstResponder(&m_selectableListView);
  m_resultsDataSource.resetMemoization();
  m_contentView.reload();
}

ViewController::TitlesDisplay ResultsController::titlesDisplay() {
  if (m_statistic->subApp() == Statistic::SubApp::Interval ||
      (m_statistic->significanceTestType() ==
           SignificanceTestType::Categorical &&
       m_statistic->categoricalType() == CategoricalType::GoodnessOfFit)) {
    return ViewController::TitlesDisplay::DisplayLastTwoTitles;
  }
  return ViewController::TitlesDisplay::DisplayLastThreeTitles;
}

const char *ResultsController::title() {
  m_titleBuffer[0] = 0;
  StackViewController *stackViewControllerResponder =
      static_cast<StackViewController *>(parentResponder());
  bool resultsIsTopPage =
      stackViewControllerResponder->topViewController() != this;
  if (resultsIsTopPage &&
      m_statistic->subApp() == Statistic::SubApp::Interval) {
    m_intervalGraphController->setResultTitleForCurrentValues(
        m_titleBuffer, sizeof(m_titleBuffer), resultsIsTopPage);
  } else {
    m_statistic->setResultTitle(m_titleBuffer, sizeof(m_titleBuffer),
                                resultsIsTopPage);
  }
  if (m_titleBuffer[0] == 0) {
    return nullptr;
  }
  return m_titleBuffer;
}

bool ResultsController::ButtonAction(ResultsController *controller, void *s) {
  if (!controller->m_statistic->isGraphable()) {
    App::app()->displayWarning(I18n::Message::InvalidValues);
    return false;
  }
  Escher::ViewController *graph;
  if (controller->m_statistic->subApp() == Statistic::SubApp::Test) {
    graph = controller->m_testGraphController;
  } else {
    graph = controller->m_intervalGraphController;
  }
  controller->stackOpenPage(graph);
  return true;
}

}  // namespace Inference
