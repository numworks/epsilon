#include "results_homogeneity_controller.h"

namespace Inference {

// ResultsTableController

ResultsHomogeneityController::ResultsTableController::ResultsTableController(Responder * parent, Escher::ViewController * resultsController, HomogeneityTest * statistic) :
  CategoricalController(parent, resultsController, Invocation(&CategoricalController::ButtonAction, this)),
  m_resultHomogeneityTable(&m_selectableTableView, this, statistic)
{
  m_selectableTableView.setTopMargin(0);
}

void ResultsHomogeneityController::ResultsTableController::didBecomeFirstResponder() {
  m_selectableTableView.reloadData(false, false);
  CategoricalController::didBecomeFirstResponder();
}

bool ResultsHomogeneityController::ResultsTableController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up) {
    m_resultHomogeneityTable.tableView()->deselectTable();
    Escher::Container::activeApp()->setFirstResponder(parentResponder());
    return true;
  }
  return typedParent()->popFromStackViewControllerOnLeftEvent(event);
}

// SingleModeController

void ResultsHomogeneityController::SingleModeController::switchToTableWithMode(ResultHomogeneityTableCell::Mode mode) {
  ResultsTableController * tableController = &(typedParent()->m_tableController);
  tableController->setMode(mode);
  Escher::Container::activeApp()->setFirstResponder(tableController);
}

// ResultsHomogeneityController

ResultsHomogeneityController::ResultsHomogeneityController(Escher::StackViewController * parent, Escher::ViewController * nextController, HomogeneityTest * statistic) :
  TabViewController(parent, this, &m_expectedValuesController, &m_contributionsController, nullptr),
  m_tableController(this, nextController, statistic),
  m_expectedValuesController(this),
  m_contributionsController(this)
{}

}
