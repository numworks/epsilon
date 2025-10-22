#include "dataset_controller.h"

#include "controller_container.h"
#include "inference_controller.h"

namespace Inference {

DatasetController::DatasetController(Escher::StackViewController* parent,
                                     ControllerContainer* controllerContainer,
                                     InferenceModel* inference)
    : InferenceController(inference, controllerContainer),
      UniformSelectableListController(parent) {
  cell(k_indexOfInputStatisticsCell)
      ->label()
      ->setMessage(I18n::Message::InputStatistics);
  cell(k_indexOfDatasetCell)->label()->setMessage(I18n::Message::UseADataSet);
}

bool DatasetController::handleEvent(Ion::Events::Event event) {
  if (!cell(0)->canBeActivatedByEvent(event)) {
    return popFromStackViewControllerOnLeftEvent(event);
  }
  int row = selectedRow();

  if (m_inferenceModel->testType() == TestType::ANOVA) {
    // TODO: open the next ANOVA controller
    return true;
  }

  InputTable* tableModel = m_inferenceModel->table();
  if (row == k_indexOfInputStatisticsCell) {
    tableModel->unsetSeries(m_inferenceModel);
    stackOpenPage(&m_controllerContainer->m_inputController);
  } else {
    assert(row == k_indexOfDatasetCell);
    m_controllerContainer->m_inputStoreController1.initSeriesSelection();
    stackOpenPage(&m_controllerContainer->m_inputStoreController1);
  }
  return true;
}

}  // namespace Inference
