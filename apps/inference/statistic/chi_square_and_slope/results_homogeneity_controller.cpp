#include "results_homogeneity_controller.h"

using namespace Escher;

namespace Inference {

// ResultsHomogeneityController

ResultsHomogeneityController::ResultsHomogeneityController(
    Escher::StackViewController* parent, Escher::ViewController* nextController,
    HomogeneityTest* statistic)
    : TabViewController(parent, this, &m_expectedValuesController,
                        &m_contributionsController, nullptr),
      m_tableController(nextController, statistic),
      m_expectedValuesController(this, &m_tableController),
      m_contributionsController(this, &m_tableController) {
  TabViewController::initView();
}

// ResultsTableController

ResultsTableController::ResultsTableController(
    Escher::ViewController* resultsController, HomogeneityTest* statistic)
    : CategoricalController(nullptr, resultsController,
                            Invocation::Builder<CategoricalController>(
                                &CategoricalController::ButtonAction, this)),
      m_resultHomogeneityTable(&m_selectableListView, statistic, this) {}

void ResultsTableController::viewWillAppear() {
  m_resultHomogeneityTable.selectableTableView()->selectRow(-1);
  m_selectableListView.selectRow(-1);
  m_selectableListView.reloadData(false);
  CategoricalController::viewWillAppear();
}

bool ResultsTableController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up) {
    m_resultHomogeneityTable.selectableTableView()->deselectTable();
    tabController()->selectTab();
    return true;
  }
  return false;
}

// SingleModeController

void ResultsHomogeneityController::SingleModeController::switchToTableWithMode(
    ResultHomogeneityTableCell::Mode mode) {
  m_tableController->setMode(mode);
  m_tableController->setParentResponder(this);
}

}  // namespace Inference
