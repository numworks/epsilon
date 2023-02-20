#include "simple_interactive_curve_view_controller.h"
#include <escher/text_field.h>
#include <cmath>
#include <assert.h>

using namespace Poincare;
using namespace Escher;

namespace Shared {

bool SimpleInteractiveCurveViewController::handleEvent(Ion::Events::Event event) {
  if (curveView()->hasFocus() &&
      (event == Ion::Events::Left || event == Ion::Events::Right)) {
    return handleLeftRightEvent(event);
  }
  return ZoomCurveViewController::handleEvent(event);
}

bool SimpleInteractiveCurveViewController::textFieldDidReceiveEvent(AbstractTextField * textField, Ion::Events::Event event) {
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && !textField->isEditing()) {
    return handleEnter();
  }
  return TextFieldDelegate::textFieldDidReceiveEvent(textField, event);
}

bool SimpleInteractiveCurveViewController::handleLeftRightEvent(Ion::Events::Event event) {
  if (moveCursorHorizontally(OMG::NewDirection(event), Ion::Events::longPressFactor())) {
    reloadBannerView();
    bool moved = interactiveCurveViewRange()->panToMakePointVisible(
      m_cursor->x(), m_cursor->y(),
      cursorTopMarginRatio(), cursorRightMarginRatio(), cursorBottomMarginRatio(), cursorLeftMarginRatio(),
      curveView()->pixelWidth()
    );
    /* Restart drawing of interrupted curves when the window pans. */
    curveView()->reload(moved);
    return true;
  }
  return false;
}

}
