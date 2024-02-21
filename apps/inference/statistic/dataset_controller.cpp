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

  RawDataStatistic* rawDataStat = rawDataStatistic();
  if (row == k_indexOfInputStatisticsCell) {
    rawDataStat->unsetSeries(m_statistic);
    stackOpenPage(m_inputController);
  } else {
    assert(row == k_indexOfDatasetCell);
    if (!rawDataStat->hasSeries()) {
      rawDataStat->setSeriesAt(m_statistic, 0, 0);
      if (rawDataStat->numberOfSeries() > 1) {
        rawDataStat->setSeriesAt(m_statistic, 1, 1);
      }
    }
    stackOpenPage(m_storeController);
  }
  return true;
}

RawDataStatistic* DatasetController::rawDataStatistic() const {
  bool intervalApp = m_statistic->subApp() == Statistic::SubApp::Interval;
  bool oneMean =
      m_statistic->significanceTestType() == SignificanceTestType::OneMean;

  if (intervalApp) {
    if (oneMean) {
      return static_cast<OneMeanInterval*>(m_statistic);
    }
    return static_cast<TwoMeansInterval*>(m_statistic);
  }
  if (oneMean) {
    return static_cast<OneMeanTest*>(m_statistic);
  }
  return static_cast<TwoMeansTest*>(m_statistic);
}

}  // namespace Inference
