#include "zoom_curve_view_controller.h"
#include <cmath>
#include <assert.h>

using namespace Poincare;

namespace Shared {

bool ZoomCurveViewController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Plus || event == Ion::Events::Minus) {
    return handleZoom(event);
  }
  return false;
}

bool ZoomCurveViewController::handleZoom(Ion::Events::Event event) {
  float ratio = event == Ion::Events::Plus ? 2.0f/3.0f : 3.0f/2.0f;
  interactiveCurveViewRange()->zoom(ratio, xFocus(), yFocus());
  curveView()->reload();
  return true;
}

}
