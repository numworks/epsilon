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

ResultsHomogeneityController::ResultsTableController::ResultsTableController(
    Escher::ViewController* resultsController, HomogeneityTest* statistic)
    : CategoricalController(nullptr, resultsController,
                            Invocation::Builder<CategoricalController>(
                                &CategoricalController::ButtonAction, this)),
      m_resultHomogeneityTable(&m_selectableListView, this, statistic) {}

void ResultsHomogeneityController::ResultsTableController::viewWillAppear() {
  m_selectableListView.reloadData(false);
  CategoricalController::viewWillAppear();
}

bool ResultsHomogeneityController::ResultsTableController::handleEvent(
    Ion::Events::Event event) {
  if (event == Ion::Events::Up) {
    m_resultHomogeneityTable.tableView()->deselectTable();
    tabController()->selectTab();
    return true;
  }
  return false;
}

void ResultsHomogeneityController::ResultsTableController::
    tableViewDidChangeSelection(SelectableTableView* t, int previousSelectedCol,
                                int previousSelectedRow,
                                bool withinTemporarySelection) {
  if (m_resultHomogeneityTable.unselectTopLeftCell(t, previousSelectedCol,
                                                   previousSelectedRow) &&
      t->selectedColumn() == 0) {
    m_resultHomogeneityTable.tableView()->deselectTable();
    tabController()->selectTab();
  }
}

// SingleModeController

void ResultsHomogeneityController::SingleModeController::switchToTableWithMode(
    ResultHomogeneityTableCell::Mode mode) {
  m_tableController->setMode(mode);
  m_tableController->setParentResponder(this);
}

}  // namespace Inference
