#include "results_homogeneity_controller.h"

using namespace Probability;

namespace Probability {

ResultsHomogeneityController::ResultsHomogeneityController(StackViewController * parent, Escher::ViewController * resultsController, HomogeneityTest * statistic) :
  CategoricalController(parent, resultsController, Invocation(&CategoricalController::ButtonAction, this)),
  m_resultHomogeneityTable(&m_selectableTableView, this, statistic)
{
  m_selectableTableView.setMargins(k_topMargin, Metric::CommonRightMargin, k_topMargin, Metric::CommonLeftMargin);
}

}
