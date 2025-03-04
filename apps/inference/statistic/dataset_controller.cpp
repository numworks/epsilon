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

  Table* tableModel = m_statistic->table();
  if (row == k_indexOfInputStatisticsCell) {
    tableModel->unsetSeries(m_statistic);
    stackOpenPage(m_inputController);
  } else {
    assert(row == k_indexOfDatasetCell);
    m_storeController->initSeriesSelection();
    stackOpenPage(m_storeController);
  }
  return true;
}

}  // namespace Inference
