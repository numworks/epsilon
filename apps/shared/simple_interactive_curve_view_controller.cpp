#include "simple_interactive_curve_view_controller.h"
#include <cmath>
#include <assert.h>

using namespace Poincare;

namespace Shared {

SimpleInteractiveCurveViewController::SimpleInteractiveCurveViewController(Responder * parentResponder,InteractiveCurveViewRange * interactiveRange, CurveView * curveView, CurveViewCursor * cursor) :
  ViewController(parentResponder),
  m_cursor(cursor)
{
}

View * SimpleInteractiveCurveViewController::view() {
  return curveView();
}

bool SimpleInteractiveCurveViewController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Plus || event == Ion::Events::Minus) {
    return handleZoom(event);
  }
  if (event == Ion::Events::Left || event == Ion::Events::Right) {
    return handleLeftRightEvent(event);
  }
  if (event == Ion::Events::EXE || event == Ion::Events::OK) {
    return handleEnter();
  }
  return false;
}

bool SimpleInteractiveCurveViewController::handleZoom(Ion::Events::Event event) {
  float ratio = event == Ion::Events::Plus ? 2.0f/3.0f : 3.0f/2.0f;
  interactiveCurveViewRange()->zoom(ratio, m_cursor->x(), m_cursor->y());
  curveView()->reload();
  return true;
}

bool SimpleInteractiveCurveViewController::handleLeftRightEvent(Ion::Events::Event event) {
  int direction = event == Ion::Events::Left ? -1 : 1;
  if (moveCursorHorizontally(direction)) {
    reloadBannerView();
    curveView()->reload();
    return true;
  }
  return false;
}

}
