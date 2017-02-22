#include "curve_parameter_controller.h"
#include <assert.h>

using namespace Shared;

namespace Graph {

CurveParameterController::CurveParameterController(InteractiveCurveViewRange * graphRange, BannerView * bannerView, CurveViewCursor * cursor) :
  FunctionCurveParameterController(graphRange, cursor),
  m_bannerView(bannerView),
  m_calculationCell(PointerTableCellWithChevron((char*)"Calculer")),
  m_derivativeCell(PointerTableCellWithSwitch((char*)"Nombre derivee"))
{
}

const char * CurveParameterController::title() const {
  return "Options de la courbe";
}

void CurveParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (cell == &m_derivativeCell) {
    SwitchView * switchView = (SwitchView *)m_derivativeCell.accessoryView();
    switchView->setState(m_bannerView->displayDerivative());
  }
}

bool CurveParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK) {
    switch (m_selectableTableView.selectedRow()) {
      case 0:
        return true;
      case 1:
        return handleGotoSelection();
      case 2:
      {
        m_bannerView->setDisplayDerivative(!m_bannerView->displayDerivative());
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

}
