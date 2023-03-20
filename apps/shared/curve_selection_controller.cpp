#include "curve_selection_controller.h"

#include "interactive_curve_view_controller.h"

using namespace Escher;

namespace Shared {

void CurveSelectionCell::drawRect(KDContext* ctx, KDRect rect) const {
  TableCell::drawRect(ctx, rect);
  // Draw the color indicator
  assert(m_color != KDColorBlack);  // Check that the color was set
  ctx->fillRect(KDRect(0, 0, k_colorIndicatorThickness, bounds().height()),
                m_color);
}

void CurveSelectionCell::setHighlighted(bool highlight) {
  TableCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight ? Palette::Select : KDColorWhite;
  m_expressionView.setBackgroundColor(backgroundColor);
}

void CurveSelectionCell::setLayout(Poincare::Layout layout) {
  m_expressionView.setLayout(layout);
  if (!layout.isUninitialized()) {
    layoutSubviews();
  }
}

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
