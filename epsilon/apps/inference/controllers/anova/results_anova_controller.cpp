#include "results_anova_controller.h"

#include "inference/controllers/controller_container.h"

using namespace Escher;

namespace Inference {

ResultsANOVAController::ResultsANOVAController(
    Escher::StackViewController* parent,
    ControllerContainer* controllerContainer, ANOVATest* inferenceModel)
    : CategoricalController(parent, &controllerContainer->m_testGraphController,
                            Invocation::Builder<CategoricalController>(
                                &CategoricalController::ButtonAction, this)),
      m_resultsBetweenWithinTable(&m_selectableListView, inferenceModel, this,
                                  this) {}

void ResultsANOVAController::createDynamicCells() {
  m_resultsBetweenWithinTable.createCells();
}

}  // namespace Inference
