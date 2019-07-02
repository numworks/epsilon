#include "curve_parameter_controller.h"
#include "graph_controller.h"
#include <apps/i18n.h>
#include <assert.h>

using namespace Shared;

namespace Graph {

CurveParameterController::CurveParameterController(InputEventHandlerDelegate * inputEventHandlerDelegate, InteractiveCurveViewRange * graphRange, BannerView * bannerView, CurveViewCursor * cursor, GraphView * graphView, GraphController * graphController) :
  FunctionCurveParameterController(),
  m_goToParameterController(this, inputEventHandlerDelegate, graphRange, cursor, I18n::Message::X),
  m_graphController(graphController),
  m_calculationCell(I18n::Message::Compute),
  m_derivativeCell(I18n::Message::DerivateNumber),
  m_calculationParameterController(this, inputEventHandlerDelegate, graphView, bannerView, graphRange, cursor)
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
  if (event == Ion::Events::OK || event == Ion::Events::EXE || (event == Ion::Events::Right && (selectedRow() == 0 || selectedRow() == 1))) {
    switch (selectedRow()) {
      case 0:
      {
        m_calculationParameterController.setRecord(m_record);
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
