#include "graph_options_controller.h"
#include <assert.h>

using namespace Shared;

namespace Regression {

GraphOptionsController::GraphOptionsController(Responder * parentResponder, Store * store, CurveViewCursor * cursor, GraphController * graphController) :
  ViewController(parentResponder),
  m_selectableTableView(this),
  m_goToParameterController(this, store, cursor, graphController)
{
}

const char * GraphOptionsController::title() {
  return I18n::translate(I18n::Message::RegressionSlope);
}

View * GraphOptionsController::view() {
  return &m_selectableTableView;
}

void GraphOptionsController::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectCellAtLocation(0, 0);
  }
  app()->setFirstResponder(&m_selectableTableView);
}

bool GraphOptionsController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    m_goToParameterController.setXPrediction(selectedRow() == 0);
    StackViewController * stack = (StackViewController *)parentResponder();
    stack->push(&m_goToParameterController);
    return true;
  }
  return false;
}

int GraphOptionsController::numberOfRows() {
  return k_totalNumberOfCells;
};

HighlightCell * GraphOptionsController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCells);
  return &m_cells[index];
}

int GraphOptionsController::reusableCellCount() {
  return k_totalNumberOfCells;
}

KDCoordinate GraphOptionsController::cellHeight() {
  return Metric::ParameterCellHeight;
}

void GraphOptionsController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  MessageTableCellWithChevron * myCell = (MessageTableCellWithChevron *)cell;
  I18n::Message  titles[2] = {I18n::Message::XPrediction, I18n::Message::YPrediction};
  myCell->setMessage(titles[index]);
}

}
