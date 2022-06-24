#include "graph_options_controller.h"
#include "app.h"
#include "graph_controller.h"
#include "regression_controller.h"
#include <assert.h>

using namespace Shared;

namespace Regression {

GraphOptionsController::GraphOptionsController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, Store * store, CurveViewCursor * cursor, GraphController * graphController) :
  ViewController(parentResponder),
  m_changeRegressionCell(I18n::Message::Regression),
  m_selectableTableView(this),
  m_goToParameterController(this, inputEventHandlerDelegate, store, cursor, graphController),
  m_store(store),
  m_graphController(graphController)
{
}

const char * GraphOptionsController::title() {
  return I18n::translate(I18n::Message::RegressionCurve);
}

View * GraphOptionsController::view() {
  return &m_selectableTableView;
}

void GraphOptionsController::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectCellAtLocation(0, 0);
  }
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

void GraphOptionsController::viewWillAppear() {
  m_selectableTableView.reloadData();
}

bool GraphOptionsController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    if (selectedRow() == numberOfRows() -1) {
      RegressionController * regressionController = App::app()->regressionController();
      regressionController->setSeries(m_graphController->selectedSeriesIndex());
      StackViewController * stack = static_cast<StackViewController *>(parentResponder());
      stack->push(regressionController);
    } else {
      m_goToParameterController.setXPrediction(selectedRow() == 0);
      StackViewController * stack = (StackViewController *)parentResponder();
      stack->push(&m_goToParameterController);
    }
    return true;
  }
  return false;
}

int GraphOptionsController::numberOfRows() const {
  return k_numberOfParameterCells + 1;
}

KDCoordinate GraphOptionsController::rowHeight(int j) {
  if ((j == numberOfRows() - 1) &&
      (static_cast<Store *>(m_store)->seriesRegressionType(m_graphController->selectedSeriesIndex()) == Model::Type::Logistic))
  {
    return RegressionController::k_logisticCellHeight;
  }
  return Metric::ParameterCellHeight;
}

KDCoordinate GraphOptionsController::cumulatedHeightFromIndex(int j) {
  assert (j >= 0 && j <= numberOfRows());
  KDCoordinate result = 0;
  for (int i = 0; i < j; i++) {
    result+= rowHeight(i);
  }
  return result;
}

int GraphOptionsController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  int result = 0;
  int j = 0;
  int numberRows = numberOfRows();
  while (result < offsetY && j < numberRows) {
    result += rowHeight(j++);
  }
  return (result < offsetY || offsetY == 0) ? j : j - 1;
}

HighlightCell * GraphOptionsController::reusableCell(int index, int type) {
  assert(index >= 0);
  assert(index < reusableCellCount(type));
  if (type == k_regressionCellType) {
    return &m_changeRegressionCell;
  }
  assert(type == k_parameterCelltype);
  return &m_parameterCells[index];
}

int GraphOptionsController::reusableCellCount(int type) {
  if (type == k_regressionCellType) {
    return 1;
  }
  assert(type == k_parameterCelltype);
  return k_numberOfParameterCells;
}

int GraphOptionsController::typeAtLocation(int i, int j) {
  assert(i == 0);
  if (j == numberOfRows() -1) {
    return k_regressionCellType;
  }
  return k_parameterCelltype;
}

void GraphOptionsController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (index == numberOfRows() - 1) {
    m_changeRegressionCell.setLayout(static_cast<Store *>(m_store)->modelForSeries(m_graphController->selectedSeriesIndex())->layout());
    return;
  }
  assert(index >=0 && index < k_numberOfParameterCells);
  MessageTableCellWithChevron<> * myCell = (MessageTableCellWithChevron<> *)cell;
  I18n::Message titles[k_numberOfParameterCells] = {I18n::Message::XPrediction, I18n::Message::YPrediction};
  myCell->setMessage(titles[index]);
}

}
