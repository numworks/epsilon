#include "prediction_parameter_controller.h"
#include <assert.h>

using namespace Shared;

namespace Regression {

PredictionParameterController::PredictionParameterController(Responder * parentResponder, Store * store, CurveViewCursor * cursor, GraphController * graphController) :
  ViewController(parentResponder),
  m_selectableTableView(this, this, 0, 1, Metric::CommonTopMargin, Metric::CommonRightMargin,
    Metric::CommonBottomMargin, Metric::CommonLeftMargin, this),
  m_goToParameterController(this, store, cursor, graphController)
{
}

const char * PredictionParameterController::title() {
  return I18n::translate(I18n::Message::RegressionSlope);
}

View * PredictionParameterController::view() {
  return &m_selectableTableView;
}

void PredictionParameterController::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectCellAtLocation(0, 0);
  }
  app()->setFirstResponder(&m_selectableTableView);
}

bool PredictionParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    m_goToParameterController.setXPrediction(selectedRow() == 0);
    StackViewController * stack = (StackViewController *)parentResponder();
    stack->push(&m_goToParameterController);
    return true;
  }
  return false;
}

int PredictionParameterController::numberOfRows() {
  return k_totalNumberOfCells;
};

HighlightCell * PredictionParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCells);
  return &m_cells[index];
}

int PredictionParameterController::reusableCellCount() {
  return k_totalNumberOfCells;
}

KDCoordinate PredictionParameterController::cellHeight() {
  return Metric::ParameterCellHeight;
}

void PredictionParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  MessageTableCellWithChevron * myCell = (MessageTableCellWithChevron *)cell;
  I18n::Message  titles[2] = {I18n::Message::XPrediction, I18n::Message::YPrediction};
  myCell->setMessage(titles[index]);
}

}
