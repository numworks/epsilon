#include "graph_options_controller.h"
#include "app.h"
#include "graph_controller.h"
#include "regression_controller.h"
#include <assert.h>

using namespace Shared;
using namespace Escher;

namespace Regression {

GraphOptionsController::GraphOptionsController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, Store * store, CurveViewCursor * cursor, GraphController * graphController) :
  SelectableListViewController(parentResponder),
  m_changeRegressionCell(I18n::Message::Regression),
  m_goToParameterController(this, inputEventHandlerDelegate, store, cursor, graphController),
  m_store(store),
  m_graphController(graphController)
{
}

const char * GraphOptionsController::title() {
  return I18n::translate(I18n::Message::RegressionCurve);
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
  StackViewController * stack = static_cast<StackViewController *>(parentResponder());
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    if (selectedRow() == numberOfRows() -1) {
      RegressionController * regressionController = App::app()->regressionController();
      regressionController->setSeries(m_graphController->selectedSeriesIndex());
      stack->push(regressionController);
    } else {
      m_goToParameterController.setXPrediction(selectedRow() == 0);
      stack->push(&m_goToParameterController);
    }
    return true;
  }
  if (event == Ion::Events::Left
   && stack->depth() > Shared::InteractiveCurveViewController::k_graphControllerStackDepth + 1)
  {
    /* We only allow popping with Left if there is another menu beneath this
     * one. */
    stack->pop();
    return true;
  }
  return false;
}

int GraphOptionsController::numberOfRows() const {
  return k_numberOfParameterCells + 1;
}

HighlightCell * GraphOptionsController::reusableCell(int index, int type) {
  assert(index >= 0);
  assert(index < reusableCellCount(type) || type == k_regressionCellType);
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

int GraphOptionsController::typeAtIndex(int index) {
  if (index == numberOfRows() -1) {
    return k_regressionCellType;
  }
  return k_parameterCelltype;
}

void GraphOptionsController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (index == numberOfRows() - 1) {
    m_changeRegressionCell.setSubtitle(static_cast<Store *>(m_store)->modelForSeries(m_graphController->selectedSeriesIndex())->name());
    return;
  }
  assert(index >=0 && index < k_numberOfParameterCells);
  MessageTableCellWithChevron * myCell = static_cast<MessageTableCellWithChevron *>(cell);
  I18n::Message titles[k_numberOfParameterCells] = {I18n::Message::XPrediction, I18n::Message::YPrediction};
  myCell->setMessage(titles[index]);
}

}
