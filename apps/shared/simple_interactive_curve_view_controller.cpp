#include "simple_interactive_curve_view_controller.h"
#include <cmath>
#include <assert.h>

using namespace Poincare;

namespace Shared {

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

bool SimpleInteractiveCurveViewController::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && !textField->isEditing()) {
    return handleEnter();
  }
  return TextFieldDelegate::textFieldDidReceiveEvent(textField, event);
}

bool SimpleInteractiveCurveViewController::handleLeftRightEvent(Ion::Events::Event event) {
  int direction = event == Ion::Events::Left ? -1 : 1;
  if (moveCursorHorizontally(direction, Ion::Events::isLongRepetition())) {
    interactiveCurveViewRange()->panToMakePointVisible(
      m_cursor->x(), m_cursor->y(),
      cursorTopMarginRatio(), k_cursorRightMarginRatio, cursorBottomMarginRatio(), k_cursorLeftMarginRatio
    );
    reloadBannerView();
    curveView()->reload();
    return true;
  }
  return false;
}

}
