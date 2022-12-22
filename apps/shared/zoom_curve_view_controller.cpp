#include "zoom_curve_view_controller.h"
#include <escher/stack_view_controller.h>
#include <cmath>
#include <assert.h>

using namespace Poincare;

namespace Shared {

bool ZoomCurveViewController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OnOff) {
    /* If the display is turned on while all curves have been interrupted,
     * redrawing the curve view will be skipped. Since the frame info has been
     * lost on powering down, this would lead to a noisy screen. */
    curveView()->resetInterruption();
    /* Return false here as the OnOff event needs to be caught downstream for
     * the screen to be redrawn. */
    return false;
  }
  if (!curveView()->hasFocus()) {
    return false;
  }
  if (event == Ion::Events::Plus || event == Ion::Events::Minus) {
    return handleZoom(event);
  }
  if (event == Ion::Events::EXE || event == Ion::Events::OK) {
    return handleEnter();
  }
  return false;
}

bool ZoomCurveViewController::handleZoom(Ion::Events::Event event) {
  float ratio = event == Ion::Events::Plus ? 1.f / k_zoomOutRatio : k_zoomOutRatio;
  interactiveCurveViewRange()->zoom(ratio, xFocus(), yFocus());
  curveView()->reload(true);
  return true;
}

bool ZoomCurveViewController::handleEnter() {
  Escher::StackViewController * stack = static_cast<Escher::StackViewController *>(parentResponder());
  stack->pop();
  return true;
}

}
