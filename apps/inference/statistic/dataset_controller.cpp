#include "dataset_controller.h"

namespace Inference {

DatasetController::DatasetController(Escher::StackViewController* parent,
                                     InputController* inputController,
                                     InputStoreController* storeController,
                                     Statistic* statistic)
    : UniformSelectableListController(parent),
      m_inputController(inputController),
      m_storeController(storeController),
      m_statistic(statistic) {
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
  OneMeanStatistic* oneMean = oneMeanStatistic();
  if (row == k_indexOfInputStatisticsCell) {
    oneMean->setSeries(-1);
    stackOpenPage(m_inputController);
  } else {
    assert(row == k_indexOfDatasetCell);
    // Keep previous series if possible
    if (oneMean->series() < 0) {
      oneMean->setSeries(0);
    }
    stackOpenPage(m_storeController);
  }
  return true;
}

OneMeanStatistic* DatasetController::oneMeanStatistic() const {
  return m_statistic->distributionType() == DistributionType::T
             ? m_statistic->subApp() == Statistic::SubApp::Interval
                   ? static_cast<OneMeanTInterval*>(m_statistic)
                   : static_cast<OneMeanStatistic*>(
                         static_cast<OneMeanTTest*>(m_statistic))
         : m_statistic->subApp() == Statistic::SubApp::Interval
             ? static_cast<OneMeanZInterval*>(m_statistic)
             : static_cast<OneMeanStatistic*>(
                   static_cast<OneMeanZTest*>(m_statistic));
}

}  // namespace Inference
