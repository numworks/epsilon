#ifndef SHARED_SIMPLE_INTERACTIVE_CURVE_VIEW_CONTROLLER_H
#define SHARED_SIMPLE_INTERACTIVE_CURVE_VIEW_CONTROLLER_H

#include "text_field_delegate.h"
#include "zoom_curve_view_controller.h"

namespace Shared {

/* SimpleInteractiveCurveViewController is a View controller with a cursor that
 * can handles zoom in/out and left and right events. */

class SimpleInteractiveCurveViewController : public ZoomCurveViewController, public TextFieldDelegate {
public:
  SimpleInteractiveCurveViewController(Responder * parentResponder, CurveViewCursor * cursor) : ZoomCurveViewController(parentResponder), m_cursor(cursor) {}
  bool handleEvent(Ion::Events::Event event) override;
  bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) override;
protected:
  constexpr static float k_cursorRightMarginRatio = 0.04f; // (cursorWidth/2)/(graphViewWidth-1)
  constexpr static float k_cursorLeftMarginRatio = 0.04f;  // (cursorWidth/2)/(graphViewWidth-1)
  virtual float cursorTopMarginRatio() { return 0.07f; }   // (cursorHeight/2)/(graphViewHeight-1)
  virtual float cursorBottomMarginRatio() = 0;             // (cursorHeight/2+bannerHeight)/(graphViewHeight-1)
  constexpr static float k_numberOfCursorStepsInGradUnit = 5.0f;
  // ZoomCurveViewController
  float xFocus() override { return m_cursor->x(); }
  float yFocus() override { return m_cursor->y(); }
  virtual bool handleLeftRightEvent(Ion::Events::Event event);
  virtual void reloadBannerView() = 0;
  /* the result of moveCursorVertically/Horizontally means:
   * false -> the cursor cannot move in this direction
   * true -> the cursor moved */
  virtual bool moveCursorHorizontally(int direction, bool fast = false) { return false; }
  virtual bool handleEnter() = 0;
  CurveViewCursor * m_cursor;
};

}

#endif
