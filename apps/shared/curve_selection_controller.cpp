#include "curve_selection_controller.h"

#include "interactive_curve_view_controller.h"

using namespace Escher;

namespace Shared {

CurveSelectionController::CurveSelectionController(
    InteractiveCurveViewController* graphController)
    : SelectableListViewController(graphController),
      m_graphController(graphController) {}

bool CurveSelectionController::handleEvent(Ion::Events::Event event) {
  // canBeActivatedByEvent can be called on any cell with chevron
  if (reusableCell(0, k_curveSelectionCellType)->canBeActivatedByEvent(event)) {
    m_graphController->openMenuForCurveAtIndex(selectedRow());
    return true;
  }
  return false;
}

}  // namespace Shared
