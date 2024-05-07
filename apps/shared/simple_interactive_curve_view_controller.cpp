#include "simple_interactive_curve_view_controller.h"

#include <assert.h>
#include <escher/text_field.h>

#include <cmath>

using namespace Poincare;
using namespace Escher;

namespace Shared {

bool SimpleInteractiveCurveViewController::handleEvent(
    Ion::Events::Event event) {
  if (curveView()->hasFocus() &&
      (event == Ion::Events::Left || event == Ion::Events::Right)) {
    return handleLeftRightEvent(event);
  }
  return ZoomCurveViewController::handleEvent(event);
}

bool SimpleInteractiveCurveViewController::textFieldDidReceiveEvent(
    AbstractTextField* textField, Ion::Events::Event event) {
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) &&
      !textField->isEditing()) {
    return handleEnter();
  }
  return MathTextFieldDelegate::textFieldDidReceiveEvent(textField, event);
}

bool SimpleInteractiveCurveViewController::handleLeftRightEvent(
    Ion::Events::Event event) {
  if (moveCursorHorizontally(event.direction(),
                             Ion::Events::longPressFactor())) {
    reloadBannerView();
    bool moved = panToMakeCursorVisible();
    /* Restart drawing of interrupted curves when the window pans. */
    curveView()->reload(moved);
    return true;
  }
  return false;
}

}  // namespace Shared
