#include "zoom_and_pan_curve_view_controller.h"

#include <assert.h>

#include <cmath>

using namespace Poincare;

namespace Shared {

bool ZoomAndPanCurveViewController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Left || event == Ion::Events::Right ||
      event == Ion::Events::Up || event == Ion::Events::Down) {
    return handlePan(event);
  }
  return ZoomCurveViewController::handleEvent(event);
}

bool ZoomAndPanCurveViewController::handlePan(Ion::Events::Event event) {
  float x = 0.0f;
  float y = 0.0f;
  if (event == Ion::Events::Up) {
    y = yMove();
  } else if (event == Ion::Events::Down) {
    y = -yMove();
  } else if (event == Ion::Events::Left) {
    x = -xMove();
  } else {
    assert(event == Ion::Events::Right);
    x = xMove();
  }
  interactiveCurveViewRange()->panWithVector(x, y);
  /* Restart drawing interrupted curves when the window pans. */
  curveView()->reload(true);
  return true;
}

}  // namespace Shared
