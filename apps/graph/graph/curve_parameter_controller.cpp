#include "curve_parameter_controller.h"
#include "graph_controller.h"
#include "../../i18n.h"
#include <assert.h>

using namespace Shared;

namespace Graph {

CurveParameterController::CurveParameterController(InteractiveCurveViewRange * graphRange, BannerView * bannerView, CurveViewCursor * cursor, GraphView * graphView, GraphController * graphController, CartesianFunctionStore * functionStore) :
  FunctionCurveParameterController(graphRange, cursor),
  m_goToParameterController(this, graphRange, cursor, I18n::Message::X),
  m_graphController(graphController),
  m_calculationCell(I18n::Message::Compute),
  m_derivativeCell(I18n::Message::DerivateNumber),
  m_calculationParameterController(this, graphView, bannerView, graphRange, cursor, functionStore)
{
}

const char * CurveParameterController::title() {
  return I18n::translate(I18n::Message::PlotOptions);
}

void CurveParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (cell == &m_derivativeCell) {
    SwitchView * switchView = (SwitchView *)m_derivativeCell.accessoryView();
    switchView->setState(m_graphController->displayDerivativeInBanner());
  }
}

bool CurveParameterController::handleEvent(Ion::Events::Event event) {
#if FUNCTION_CALCULATE_MENU
  if (event == Ion::Events::OK || event == Ion::Events::EXE || (event == Ion::Events::Right && (selectedRow() == 0 || selectedRow() == 1))) {
#else
  if (event == Ion::Events::OK || event == Ion::Events::EXE || (event == Ion::Events::Right && selectedRow() == 0)) {
#endif
    switch (selectedRow()) {
      case 0:
      {
        m_calculationParameterController.setFunction(static_cast<CartesianFunction *>(m_function));
        StackViewController * stack = (StackViewController *)parentResponder();
        stack->push(&m_calculationParameterController);
        return true;
      }
      case 1:
        return handleGotoSelection();
      case 2:
      {
        m_graphController->setDisplayDerivativeInBanner(!m_graphController->displayDerivativeInBanner());
        m_selectableTableView.reloadData();
        return true;
      }
      default:
        return false;
    }
  }
  return false;
}

int CurveParameterController::numberOfRows() {
  return k_totalNumberOfCells;
};

HighlightCell * CurveParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCells);
  HighlightCell * cells[] = {&m_calculationCell, &m_goToCell, &m_derivativeCell};
  return cells[index];
}

int CurveParameterController::reusableCellCount() {
  return k_totalNumberOfCells;
}

FunctionGoToParameterController * CurveParameterController::goToParameterController() {
  return &m_goToParameterController;
}

}
