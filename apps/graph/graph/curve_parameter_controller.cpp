#include "curve_parameter_controller.h"
#include "graph_controller.h"
#include "../app.h"
#include <apps/i18n.h>
#include <assert.h>

using namespace Shared;
using namespace Escher;

namespace Graph {

CurveParameterController::CurveParameterController(Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, InteractiveCurveViewRange * graphRange, BannerView * bannerView, CurveViewCursor * cursor, GraphView * graphView, GraphController * graphController) :
  FunctionCurveParameterController(),
  m_goToParameterController(this, inputEventHandlerDelegate, graphRange, cursor),
  m_calculationCell(I18n::Message::Compute),
  m_calculationParameterController(this, inputEventHandlerDelegate, graphView, bannerView, graphRange, cursor, graphController)
{
}

const char * CurveParameterController::title() {
  return I18n::translate(I18n::Message::PlotOptions);
}

void CurveParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
}

bool CurveParameterController::handleEvent(Ion::Events::Event event) {
  int index;
  if (shouldDisplayCalculation()) {
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
      default:
        assert(false);
        return false;
    }
  }
  return FunctionCurveParameterController::handleEvent(event);
}

int CurveParameterController::numberOfRows() const {
  return 1 + shouldDisplayCalculation();
};

HighlightCell * CurveParameterController::reusableCell(int index) {
  assert(0 <= index && index < reusableCellCount());
  HighlightCell * cells[] = {&m_calculationCell, &m_goToCell};
  return cells[cellIndex(index)];
}

void CurveParameterController::viewWillAppear() {
  Shared::FunctionCurveParameterController::viewWillAppear();
  m_selectableTableView.reloadData();
}

bool CurveParameterController::shouldDisplayCalculation() const {
  return App::app()->functionStore()->modelForRecord(m_record)->canDisplayDerivative();
}

int CurveParameterController::cellIndex(int visibleCellIndex) const {
  if (shouldDisplayCalculation()) {
   return visibleCellIndex;
  }
  assert(visibleCellIndex == 0);
  return 1;
}

FunctionGoToParameterController * CurveParameterController::goToParameterController() {
  return &m_goToParameterController;
}

}
