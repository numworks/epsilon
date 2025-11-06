#include "results_anova_controller.h"

#include "escher/responder.h"
#include "inference/controllers/controller_container.h"
#include "inference/models/anova_test.h"

using namespace Escher;

namespace Inference {

// ResultsANOVAController

ResultsANOVAController::ResultsANOVAController(
    Escher::StackViewController* parent,
    ControllerContainer* controllerContainer, ANOVATest* inferenceModel)
    : InferenceController(inferenceModel),
      TabViewController(parent, this,
                        {&m_firstTabController, &m_secondTabController}),
      m_firstTabController(this, controllerContainer, inferenceModel),
      m_secondTabController(this, controllerContainer, inferenceModel) {}

bool ResultsANOVAController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up) {
    selectTab();
    return true;
  }
  return Escher::TabViewController::handleEvent(event);
}

// BetweenWithinController
BetweenWithinController::BetweenWithinController(
    Escher::Responder* parent, ControllerContainer* controllerContainer,
    ANOVATest* inferenceModel)
    : CategoricalController(parent, &controllerContainer->m_resultsController,
                            Invocation::Builder<CategoricalController>(
                                &CategoricalController::ButtonAction, this)),
      m_resultsBetweenWithinTable(&m_selectableListView, inferenceModel, this,
                                  this) {}

void BetweenWithinController::createDynamicCells() {
  m_resultsBetweenWithinTable.createCells();
}

// StatisticsController
StatisticsController::StatisticsController(
    Escher::Responder* parent, ControllerContainer* controllerContainer,
    ANOVATest* inferenceModel)
    : CategoricalController(parent, &controllerContainer->m_resultsController,
                            Invocation::Builder<CategoricalController>(
                                &CategoricalController::ButtonAction, this)),
      m_resultsStatisticsTable(&m_selectableListView, inferenceModel, this,
                               this) {}

void StatisticsController::createDynamicCells() {
  m_resultsStatisticsTable.createCells();
}

}  // namespace Inference
