#include "dataset_controller.h"

#include <omg/unreachable.h>

#include "controller_container.h"
#include "inference/models/aliases.h"
#include "inference_controller.h"

namespace Inference {

DatasetController::DatasetController(Escher::StackViewController* parent,
                                     ControllerContainer* controllerContainer,
                                     InferenceModel* inference)
    : InferenceController(inference, controllerContainer),
      UniformSelectableListController(parent) {}

DatasetController::Options DatasetController::getListOptions() const {
  switch (m_inferenceModel->testType()) {
    case TestType::OneMean:
    case TestType::TwoMeans:
      return Options::InputStatisticsAndInputDataset;
    case TestType::ANOVA:
      return Options::InputDataAndInputStatistics;
    default:
      // Other tests do not use the DatasetController
      OMG::unreachable();
  }
}

int DatasetController::indexOfInputStatisticsCell() const {
  switch (getListOptions()) {
    case Options::InputStatisticsAndInputDataset:
      return 0;
    case Options::InputDataAndInputStatistics:
      return 1;
    default:
      OMG::unreachable();
  }
}

int DatasetController::indexOfDatasetCell() const {
  switch (getListOptions()) {
    case Options::InputStatisticsAndInputDataset:
      return 1;
    case Options::InputDataAndInputStatistics:
      return -1;
    default:
      OMG::unreachable();
  }
}

int DatasetController::indexOfInputDataCell() const {
  switch (getListOptions()) {
    case Options::InputStatisticsAndInputDataset:
      return -1;
    case Options::InputDataAndInputStatistics:
      return 0;
    default:
      OMG::unreachable();
  }
}

void DatasetController::initView() {
  cell(indexOfInputStatisticsCell())
      ->label()
      ->setMessage(I18n::Message::InputStatistics);
  switch (getListOptions()) {
    case Options::InputStatisticsAndInputDataset: {
      cell(indexOfDatasetCell())
          ->label()
          ->setMessage(I18n::Message::UseADataSet);
      break;
    }
    case Options::InputDataAndInputStatistics: {
      cell(indexOfInputDataCell())
          ->label()
          ->setMessage(I18n::Message::InputData);
      break;
    }
  }
}

bool DatasetController::handleEvent(Ion::Events::Event event) {
  if (!cell(0)->canBeActivatedByEvent(event)) {
    return popFromStackViewControllerOnLeftEvent(event);
  }
  int row = selectedRow();

  if (row == indexOfInputStatisticsCell()) {
    if (m_inferenceModel->testType() == TestType::ANOVA) {
      // TODO: table for ANOVA model
      return true;
    }
    m_inferenceModel->table()->unsetSeries(m_inferenceModel);
    stackOpenPage(&m_controllerContainer->m_inputController);
    return true;
  }
  if (row == indexOfDatasetCell()) {
    m_controllerContainer->m_inputStoreController1.initSeriesSelection();
    stackOpenPage(&m_controllerContainer->m_inputStoreController1);
    return true;
  }
  if (row == indexOfInputDataCell()) {
    if (m_inferenceModel->testType() == TestType::ANOVA) {
      // TODO: table for ANOVA model
      return true;
    }
    stackOpenPage(&m_controllerContainer->m_inputHomogeneityController);
    return true;
  }
  OMG::unreachable();
}

}  // namespace Inference
