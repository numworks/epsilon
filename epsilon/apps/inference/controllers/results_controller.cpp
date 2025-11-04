#include "results_controller.h"

#include <poincare/print.h>

#include "inference/app.h"
#include "inference/controllers/inference_controller.h"
#include "inference/text_helpers.h"

using namespace Escher;

namespace Inference {

ResultsController::ResultsController(Responder* parent,
                                     InferenceModel* inference,
                                     ControllerContainer* controllerContainer,
                                     bool enableHeadline)
    : InferenceController(inference),
      ListWithTopAndBottomController(parent,
                                     enableHeadline ? &m_title : nullptr),
      m_title(I18n::Message::CalculatedValues, k_messageFormat),
      m_controllerContainer(controllerContainer),
      m_titleBuffer{0},
      m_next(&m_selectableListView, I18n::Message::Next,
             Invocation::Builder<ResultsController>(
                 &ResultsController::ButtonAction, this),
             ButtonCell::Style::EmbossedLight) {}

ViewController::TitlesDisplay ResultsController::titlesDisplay() const {
  if (m_inferenceModel->subApp() == SubApp::ConfidenceInterval ||
      (m_inferenceModel->testType() == TestType::Chi2 &&
       m_inferenceModel->categoricalType() == CategoricalType::GoodnessOfFit)) {
    return ViewController::TitlesDisplay::DisplayLastTwoTitles;
  }
  return m_inferenceModel->canChooseDataset()
             ? TitlesDisplay(0b1011)
             : ViewController::TitlesDisplay::DisplayLastThreeTitles;
}

const char* ResultsController::title() const {
  m_titleBuffer[0] = 0;
  StackViewController* stackViewControllerResponder =
      static_cast<StackViewController*>(parentResponder());
  bool resultsIsTopPage =
      stackViewControllerResponder->topViewController() != this;
  if (resultsIsTopPage &&
      m_inferenceModel->subApp() == SubApp::ConfidenceInterval) {
    m_controllerContainer->m_intervalGraphController
        .setResultTitleForCurrentValues(m_titleBuffer, sizeof(m_titleBuffer),
                                        resultsIsTopPage);
  } else {
    m_inferenceModel->setResultTitle(m_titleBuffer, sizeof(m_titleBuffer),
                                     resultsIsTopPage);
  }
  if (m_titleBuffer[0] == 0) {
    return nullptr;
  }
  return m_titleBuffer;
}

void ResultsController::initView() {
  createCells();
  ListWithTopAndBottomController::initView();
}

bool ResultsController::ButtonAction(ResultsController* controller, void* s) {
  if (!controller->m_inferenceModel->isGraphable()) {
    App::app()->displayWarning(I18n::Message::InvalidValues);
    return false;
  }
  ViewController* graph;
  if (controller->m_inferenceModel->subApp() == SubApp::SignificanceTest) {
    graph = &controller->m_controllerContainer->m_testGraphController;
  } else {
    graph = &controller->m_controllerContainer->m_intervalGraphController;
  }
  controller->stackOpenPage(graph);
  return true;
}

int ResultsController::numberOfRows() const {
  return m_inferenceModel->numberOfResults() + 1 /* button */;
}

void ResultsController::fillCellForRow(HighlightCell* cell, int row) {
  if (typeAtRow(row) != k_resultCellType) {
    return;
  }
  ResultCell* myCell = static_cast<ResultCell*>(cell);
  double value;
  Poincare::Layout message;
  I18n::Message subMessage;
  int precision = Poincare::Preferences::VeryLargeNumberOfSignificantDigits;
  m_inferenceModel->resultAtIndex(row, &value, &message, &subMessage,
                                  &precision);
  constexpr int bufferSize = Constants::k_largeBufferSize;
  char buffer[bufferSize];
  Shared::PoincareHelpers::ConvertFloatToTextWithDisplayMode(
      value, buffer, bufferSize, precision,
      Poincare::Preferences::PrintFloatMode::Decimal);
  myCell->label()->setLayout(message);
  myCell->subLabel()->setMessage(subMessage);
  myCell->accessory()->setText(buffer);
}

HighlightCell* ResultsController::reusableCell(int index, int type) {
  if (type == k_resultCellType) {
    return cell(index);
  }
  assert(type == k_buttonCellType);
  return &m_next;
}

int ResultsController::reusableCellCount(int type) const {
  if (type == k_resultCellType) {
    return k_numberOfReusableCells;
  }
  return 1;
}

int ResultsController::typeAtRow(int row) const {
  if (row == numberOfRows() - 1) {
    return k_buttonCellType;
  }
  return k_resultCellType;
}

KDCoordinate ResultsController::nonMemoizedRowHeight(int row) {
  if (typeAtRow(row) == k_resultCellType) {
    ResultCell tempCell;
    return protectedNonMemoizedRowHeight(&tempCell, row);
  }
  assert(typeAtRow(row) == k_buttonCellType);
  return m_next.minimalSizeForOptimalDisplay().height();
}

void ResultsController::createCells() {
  if (DynamicCellsDataSource<ResultCell>::m_cells == nullptr) {
    DynamicCellsDataSource<ResultCell>::createCellsWithOffset(
        ResultsControllerDimensions::k_numberOfReusableCells, 0);
  }
}

}  // namespace Inference
