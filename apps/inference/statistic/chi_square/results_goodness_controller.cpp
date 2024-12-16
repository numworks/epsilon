#include "results_goodness_controller.h"

namespace Inference {

// ResultsGoodnessTabController

ResultsGoodnessTabController::ResultsGoodnessTabController(
    Escher::Responder* parent, TestGraphController* testGraphController,
    IntervalGraphController* intervalGraphController, GoodnessTest* statistic)
    : TabViewController(parent, this, &m_resultsController,
                        &m_contributionsController, nullptr),
      m_resultsController(this, statistic, testGraphController,
                          intervalGraphController, false),
      m_contributionsController(this, testGraphController, statistic) {}

bool ResultsGoodnessTabController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up) {
    selectTab();
    return true;
  }
  return Escher::TabViewController::handleEvent(event);
}

// ResultsGoodnessTabController::MainResultsController

const char* ResultsGoodnessTabController::MainResultsController::title() const {
  m_statistic->setResultTitle(m_titleBuffer, sizeof(m_titleBuffer), true);
  return m_titleBuffer;
}

// ResultsGoodnessTabController::ContributionsController

ResultsGoodnessTabController::ContributionsController::ContributionsController(
    Escher::Responder* parentResponder, Escher::ViewController* nextController,
    GoodnessTest* statistic)
    : CategoricalController(parentResponder, nextController,
                            Escher::Invocation::Builder<CategoricalController>(
                                &CategoricalController::ButtonAction, this)),
      m_table(&m_selectableListView, this, statistic, this) {}

void ResultsGoodnessTabController::ContributionsController::
    createDynamicCells() {
  m_table.createCells();
}

void ResultsGoodnessTabController::ContributionsController::viewWillAppear() {
  m_table.selectableTableView()->selectRow(-1);
  m_table.selectableTableView()->resetScroll();
  m_selectableListView.selectRow(-1);
  m_selectableListView.resetScroll();
  m_selectableListView.reloadData(false);
  CategoricalController::viewWillAppear();
}

}  // namespace Inference
