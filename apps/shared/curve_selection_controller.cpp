#include "curve_selection_controller.h"
#include "interactive_curve_view_controller.h"

using namespace Escher;

namespace Shared {

void CurveSelectionCell::setHighlighted(bool highlight) {
  TableCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Palette::Select : KDColorWhite;
  m_expressionView.setBackgroundColor(backgroundColor);
}

void CurveSelectionCell::setLayout(Poincare::Layout layout) {
  m_expressionView.setLayout(layout);
  if (!layout.isUninitialized()) {
    layoutSubviews();
  }
}

CurveSelectionController::CurveSelectionController(InteractiveCurveViewController * graphController) :
  Escher::ViewController(graphController),
  m_graphController(graphController),
  m_selectableTableView(this, this, this)
{}

void CurveSelectionController::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectCellAtLocation(0, 0);
  }
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool CurveSelectionController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    m_graphController->openMenuForCurveAtIndex(selectedRow());
    return true;
  }
  return false;
}

}
