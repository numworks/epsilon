#include "curve_selection_controller.h"

#include "interactive_curve_view_controller.h"

using namespace Escher;

namespace Shared {

CurveSelectionController::CurveSelectionController(
    InteractiveCurveViewController* graphController)
    : SelectableListViewController(graphController),
      m_graphController(graphController) {}

bool CurveSelectionController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE ||
      event == Ion::Events::Right) {
    m_graphController->openMenuForCurveAtIndex(selectedRow());
    return true;
  }
  return false;
}

}  // namespace Shared
