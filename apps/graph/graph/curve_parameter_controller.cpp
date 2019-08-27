#include "curve_parameter_controller.h"
#include "graph_controller.h"
#include "../app.h"
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
  int index = cellIndex(selectedRow());
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
        return false;
    }
  }
  return false;
}

int CurveParameterController::numberOfRows() {
  return reusableCellCount();
};

HighlightCell * CurveParameterController::reusableCell(int index) {
  assert(0 <= index && index < reusableCellCount());
  HighlightCell * cells[] = {&m_calculationCell, &m_goToCell, &m_derivativeCell};
  return cells[cellIndex(index)];
}

int CurveParameterController::reusableCellCount() {
  Shared::ExpiringPointer<CartesianFunction> f = App::app()->functionStore()->modelForRecord(m_record);
  return 3 - (f->plotType() != CartesianFunction::PlotType::Cartesian) * 2;
}

int CurveParameterController::cellIndex(int visibleCellIndex) const {
  Shared::ExpiringPointer<CartesianFunction> f = App::app()->functionStore()->modelForRecord(m_record);
  return (f->plotType() != CartesianFunction::PlotType::Cartesian) + visibleCellIndex;
}

FunctionGoToParameterController * CurveParameterController::goToParameterController() {
  return &m_goToParameterController;
}

}
