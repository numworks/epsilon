#include "curve_parameter_controller.h"
#include "../../i18n.h"
#include <assert.h>

using namespace Shared;

namespace Graph {

CurveParameterController::CurveParameterController(InteractiveCurveViewRange * graphRange, BannerView * bannerView, CurveViewCursor * cursor) :
  FunctionCurveParameterController(graphRange, cursor),
  m_goToParameterController(this, graphRange, cursor, I18n::Message::X),
  m_bannerView(bannerView),
#if FUNCTION_CALCULATE_MENU
  m_calculationCell(I18n::Message::Compute),
#endif
  m_derivativeCell(I18n::Message::DerivateNumber)
{
}

const char * CurveParameterController::title() {
  return I18n::translate(I18n::Message::PlotOptions);
}

void CurveParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (cell == &m_derivativeCell) {
    SwitchView * switchView = (SwitchView *)m_derivativeCell.accessoryView();
    switchView->setState(m_bannerView->displayDerivative());
  }
}

bool CurveParameterController::handleEvent(Ion::Events::Event event) {
#if FUNCTION_CALCULATE_MENU
  if (event == Ion::Events::OK || event == Ion::Events::EXE || (event == Ion::Events::Right && (selectedRow() == 0 || selectedRow() == 1))) {
#else
  if (event == Ion::Events::OK || event == Ion::Events::EXE || (event == Ion::Events::Right && selectedRow() == 0)) {
#endif
    switch (selectedRow()) {
#if FUNCTION_CALCULATE_MENU
      case 0:
        return true;
      case 1:
#else
      case 0:
#endif
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
#if FUNCTION_CALCULATE_MENU
  HighlightCell * cells[] = {&m_calculationCell, &m_goToCell, &m_derivativeCell};
#else
  HighlightCell * cells[] = {&m_goToCell, &m_derivativeCell};
#endif
  return cells[index];
}

int CurveParameterController::reusableCellCount() {
  return k_totalNumberOfCells;
}

FunctionGoToParameterController * CurveParameterController::goToParameterController() {
  return &m_goToParameterController;
}

}
