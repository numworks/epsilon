#include "curve_parameter_controller.h"
#include "graph_controller.h"
#include "../app.h"
#include <apps/i18n.h>
#include <assert.h>

using namespace Shared;

namespace Graph {

CurveParameterController::CurveParameterController(InputEventHandlerDelegate * inputEventHandlerDelegate, InteractiveCurveViewRange * graphRange, BannerView * bannerView, CurveViewCursor * cursor, GraphView * graphView, GraphController * graphController) :
  FunctionCurveParameterController(),
  m_goToParameterController(this, inputEventHandlerDelegate, graphRange, cursor),
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
  int index;
  if (shouldDisplayCalculationAndDerivative()) {
    index = selectedRow();
  } else {
    assert(selectedRow() == 0);
    index = 1;
  }
  if (event == Ion::Events::OK || event == Ion::Events::EXE || (event == Ion::Events::Right && (index == 0 || index == 1))) {
    switch (index) {
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
        assert(false);
        return false;
    }
  }
  return false;
}

int CurveParameterController::numberOfRows() const {
  return reusableCellCount();
};

HighlightCell * CurveParameterController::reusableCell(int index) {
  assert(0 <= index && index < reusableCellCount());
  HighlightCell * cells[] = {&m_calculationCell, &m_goToCell, &m_derivativeCell};
  return cells[cellIndex(index)];
}

int CurveParameterController::reusableCellCount() const {
  return 1 + (shouldDisplayCalculationAndDerivative() ? 2 : 0);
}

void CurveParameterController::viewWillAppear() {
  Shared::FunctionCurveParameterController::viewWillAppear();
  m_selectableTableView.reloadData();
}

bool CurveParameterController::shouldDisplayCalculationAndDerivative() const {
  Shared::ExpiringPointer<ContinuousFunction> f = App::app()->functionStore()->modelForRecord(m_record);
  return f->plotType() == ContinuousFunction::PlotType::Cartesian;
}

int CurveParameterController::cellIndex(int visibleCellIndex) const {
  if (shouldDisplayCalculationAndDerivative()) {
   return visibleCellIndex;
  }
  assert(visibleCellIndex == 0);
  return 1;
}

FunctionGoToParameterController * CurveParameterController::goToParameterController() {
  return &m_goToParameterController;
}

}
