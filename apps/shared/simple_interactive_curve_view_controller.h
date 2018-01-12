#ifndef SHARED_SIMPLE_INTERACTIVE_CURVE_VIEW_CONTROLLER_H
#define SHARED_SIMPLE_INTERACTIVE_CURVE_VIEW_CONTROLLER_H

#include <escher.h>
#include "interactive_curve_view_range.h"
#include "curve_view_cursor.h"
#include "curve_view.h"

namespace Shared {

/* SimpleInteractiveCurveViewController is a View controller with a cursor that
 * can handles zoom in/out and left and right events. */

class SimpleInteractiveCurveViewController : public ViewController {
public:
  SimpleInteractiveCurveViewController(Responder * parentResponder, InteractiveCurveViewRange * interactiveRange, CurveView * curveView, CurveViewCursor * cursor);
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
  constexpr static float k_cursorRightMarginRatio = 0.04f; // (cursorWidth/2)/graphViewWidth
  constexpr static float k_cursorLeftMarginRatio = 0.04f;  // (cursorWidth/2)/graphViewWidth
  constexpr static float k_numberOfCursorStepsInGradUnit = 5.0f;
protected:
  virtual bool handleZoom(Ion::Events::Event event);
  virtual bool handleLeftRightEvent(Ion::Events::Event event);
  virtual void reloadBannerView() {};
  /* the result of moveCursorVertically/Horizontally means:
   * false -> the cursor cannot move in this direction
   * true -> the cursor moved */
  virtual bool moveCursorHorizontally(int direction) { return false; };
  virtual InteractiveCurveViewRange * interactiveCurveViewRange() = 0;
  virtual CurveView * curveView() = 0;
  virtual bool handleEnter() = 0;
  CurveViewCursor * m_cursor;
};

}

#endif
