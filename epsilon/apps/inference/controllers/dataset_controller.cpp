#include "dataset_controller.h"

#include <omg/unreachable.h>

#include "controller_container.h"
#include "inference/models/aliases.h"
#include "inference_controller.h"

namespace Inference {

void DatasetControllerCell::updateLabelAndReload(I18n::Message message) {
  label()->setMessage(message);
  // Re-layout subviews
  layoutSubviews();
}

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
      OMG_UNREACHABLE;
  }
}

int DatasetController::indexOfInputStatisticsCell() const {
  switch (getListOptions()) {
    case Options::InputStatisticsAndInputDataset:
      return 0;
    case Options::InputDataAndInputStatistics:
      return 1;
    default:
      OMG_UNREACHABLE;
  }
}

int DatasetController::indexOfDatasetCell() const {
  switch (getListOptions()) {
    case Options::InputStatisticsAndInputDataset:
      return 1;
    case Options::InputDataAndInputStatistics:
      return -1;
    default:
      OMG_UNREACHABLE;
  }
}

int DatasetController::indexOfInputDataCell() const {
  switch (getListOptions()) {
    case Options::InputStatisticsAndInputDataset:
      return -1;
    case Options::InputDataAndInputStatistics:
      return 0;
    default:
      OMG_UNREACHABLE;
  }
}

void DatasetController::initView() {
  cell(indexOfInputStatisticsCell())
      ->updateLabelAndReload(I18n::Message::InputStatistics);
  switch (getListOptions()) {
    case Options::InputStatisticsAndInputDataset: {
      cell(indexOfDatasetCell())
          ->updateLabelAndReload(I18n::Message::UseADataSet);
      break;
    }
    case Options::InputDataAndInputStatistics: {
      cell(indexOfInputDataCell())
          ->updateLabelAndReload(I18n::Message::InputData);
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
      stackOpenPage(&m_controllerContainer->m_inputStatisticsController);
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
      stackOpenPage(&m_controllerContainer->m_inputDataController);
      return true;
    }
    stackOpenPage(&m_controllerContainer->m_inputHomogeneityController);
    return true;
  }
  OMG_UNREACHABLE;
}

}  // namespace Inference
