#include "type_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <escher/view_controller.h>
#include <ion/events.h>
#include <poincare/print.h>
#include <poincare/statistics/inference.h>

#include "controller_container.h"
#include "inference/controllers/inference_controller.h"
#include "inference/models/inference_model.h"

using namespace Escher;

namespace Inference {

TypeController::TypeController(StackViewController* parent,
                               ControllerContainer* controllerContainer,
                               InferenceModel* inference)
    : InferenceController(inference, controllerContainer),
      UniformSelectableListController(parent) {
  m_selectableListView.margins()->setBottom(0);
  // Init selection
  selectRow(0);
}

void TypeController::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    m_selectableListView.reloadData();
  } else {
    UniformSelectableListController::handleResponderChainEvent(event);
  }
}

bool TypeController::handleEvent(Ion::Events::Event event) {
  // canBeActivatedByEvent can be called on any cell with chevron
  if (!cell(0)->canBeActivatedByEvent(event)) {
    return popFromStackViewControllerOnLeftEvent(event);
  }
  StatisticType type;
  int selRow = selectedRow();
  if (selRow == k_indexOfTTest) {
    type = StatisticType::T;
  } else if (selRow == k_indexOfZTest) {
    type = StatisticType::Z;
  } else {
    assert(selRow == k_indexOfPooledTest);
    type = StatisticType::TPooled;
  }
  ViewController* controller = &m_controllerContainer->m_inputController;
  if (m_inferenceModel->hasHypothesisParameters()) {
    controller = &m_controllerContainer->m_hypothesisEditableController;
  } else if (m_inferenceModel->canChooseDataset()) {
    /* Reset row of DatasetController here and not in
     * viewWillAppear or initView because we want
     * to save row when we come back from results. */
    m_controllerContainer->m_datasetController.selectRow(0);
    controller = &m_controllerContainer->m_datasetController;
  }
  assert(controller != nullptr);
  m_inferenceModel->initializeDistribution(type);
  stackOpenPage(controller);
  return true;
}

const char* TypeController::title() const {
  I18n::Message format = m_inferenceModel->distributionTitle();
  I18n::Message testOrInterval = m_inferenceModel->subAppBasicTitle();
  Poincare::Print::CustomPrintf(m_titleBuffer, sizeof(m_titleBuffer),
                                I18n::translate(format),
                                I18n::translate(testOrInterval));
  return m_titleBuffer;
}

void TypeController::stackOpenPage(ViewController* nextPage) {
  switch (m_inferenceModel->statisticType()) {
    case StatisticType::T:
      selectRow(k_indexOfTTest);
      break;
    case StatisticType::TPooled:
      selectRow(k_indexOfPooledTest);
      break;
    default:
      assert(m_inferenceModel->statisticType() == StatisticType::Z);
      selectRow(k_indexOfZTest);
      break;
  }
  ViewController::stackOpenPage(nextPage);
}

void TypeController::viewWillAppear() {
  cell(k_indexOfTTest)
      ->label()
      ->setMessage(m_inferenceModel->tDistributionName());
  cell(k_indexOfPooledTest)
      ->label()
      ->setMessage(m_inferenceModel->tPooledDistributionName());
  cell(k_indexOfZTest)
      ->label()
      ->setMessage(m_inferenceModel->zDistributionName());
  cell(k_indexOfPooledTest)
      ->setVisible(m_inferenceModel->numberOfAvailableStatistics() ==
                   numberOfRows());
}

}  // namespace Inference
