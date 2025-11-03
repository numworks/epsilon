#include "results_homogeneity_controller.h"

#include "inference/controllers/controller_container.h"
#include "inference/controllers/inference_controller.h"

using namespace Escher;

namespace Inference {

// ResultsHomogeneityTabController

ResultsHomogeneityTabController::ResultsHomogeneityTabController(
    Escher::StackViewController* parent,
    ControllerContainer* controllerContainer, HomogeneityTest* inference)
    : InferenceController(inference),
      TabViewController(
          parent, this,
          {&m_expectedValuesController, &m_contributionsController}),
      m_tableController(&controllerContainer->m_resultsController, inference),
      m_expectedValuesController(this, &m_tableController),
      m_contributionsController(this, &m_tableController) {}

// ResultsHomogeneityController

ResultsHomogeneityController::ResultsHomogeneityController(
    Escher::ViewController* resultsController, HomogeneityTest* inference)
    : CategoricalController(nullptr, resultsController,
                            Invocation::Builder<CategoricalController>(
                                &CategoricalController::ButtonAction, this)),
      m_resultsHomogeneityTable(&m_selectableListView, inference, this, this) {}

void ResultsHomogeneityController::createDynamicCells() {
  m_resultsHomogeneityTable.createCells();
}

void ResultsHomogeneityController::viewWillAppear() {
  m_resultsHomogeneityTable.selectableTableView()->selectRow(-1);
  m_resultsHomogeneityTable.selectableTableView()->resetScroll();
  CategoricalController::viewWillAppear();
}

bool ResultsHomogeneityController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up) {
    m_resultsHomogeneityTable.selectableTableView()->deselectTable();
    tabController()->selectTab();
    return true;
  }
  return false;
}

// SingleModeController

void ResultsHomogeneityTabController::SingleModeController::
    switchToTableWithMode(ResultsHomogeneityTableCell::Mode mode) {
  m_tableController->setMode(mode);
  m_tableController->setParentResponder(this);
}

}  // namespace Inference
