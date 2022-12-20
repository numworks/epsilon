#include "zoom_and_pan_curve_view_controller.h"
#include <cmath>
#include <assert.h>

using namespace Poincare;

namespace Shared {

bool ZoomAndPanCurveViewController::handleEvent(Ion::Events::Event & event) {
  if (event == Ion::Events::Left || event == Ion::Events::Right || event == Ion::Events::Up || event == Ion::Events::Down) {
    return handlePan(event);
  }
  return ZoomCurveViewController::handleEvent(event);
}

bool ZoomAndPanCurveViewController::handlePan(Ion::Events::Event event) {
  float xMove = 0.0f;
  float yMove = 0.0f;
  if (event == Ion::Events::Up) {
    yMove = interactiveCurveViewRange()->yGridUnit();
  } else if (event == Ion::Events::Down) {
    yMove = -interactiveCurveViewRange()->yGridUnit();
  } else if (event == Ion::Events::Left) {
    xMove = -interactiveCurveViewRange()->xGridUnit();
  } else {
    assert(event == Ion::Events::Right);
    xMove = interactiveCurveViewRange()->xGridUnit();
  }
  interactiveCurveViewRange()->panWithVector(xMove, yMove);
  /* Restart drawing interrupted curves when the window pans. */
  curveView()->reload(true);
  return true;
}

}

