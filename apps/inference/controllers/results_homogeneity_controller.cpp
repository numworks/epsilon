#include "results_homogeneity_controller.h"

namespace Inference {

ResultsHomogeneityController::ResultsHomogeneityController(StackViewController * parent, Escher::ViewController * resultsController, HomogeneityTest * statistic) :
  CategoricalController(parent, resultsController, Invocation(&CategoricalController::ButtonAction, this)),
  m_resultHomogeneityTable(&m_selectableTableView, this, statistic)
{
  m_selectableTableView.setTopMargin(0);
}

void ResultsHomogeneityController::didBecomeFirstResponder() {
  m_selectableTableView.reloadData(false, false);
  CategoricalController::didBecomeFirstResponder();
}

}
