#include "zoom_curve_view_controller.h"

#include <assert.h>
#include <escher/stack_view_controller.h>

#include <cmath>

using namespace Poincare;

namespace Shared {

bool ZoomCurveViewController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Plus || event == Ion::Events::Minus) {
    return handleZoom(event);
  }
  if (event == Ion::Events::EXE || event == Ion::Events::OK) {
    return handleEnter();
  }
  return false;
}

void ZoomCurveViewController::viewWillAppear() {
  interactiveCurveViewRange()->setOffscreenYAxis(offscreenYAxis());
  /* Force a reload in case some curves were interrupted. */
  curveView()->reload(true);
}

bool ZoomCurveViewController::handleZoom(Ion::Events::Event event) {
  float ratio =
      event == Ion::Events::Plus ? 1.f / k_zoomOutRatio : k_zoomOutRatio;
  interactiveCurveViewRange()->zoom(ratio, xFocus(), yFocus());
  curveView()->reload(true);
  return true;
}

bool ZoomCurveViewController::handleEnter() {
  Escher::StackViewController* stack =
      static_cast<Escher::StackViewController*>(parentResponder());
  stack->pop();
  return true;
}

}  // namespace Shared
