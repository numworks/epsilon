#ifndef SHARED_SIMPLE_INTERACTIVE_CURVE_VIEW_CONTROLLER_H
#define SHARED_SIMPLE_INTERACTIVE_CURVE_VIEW_CONTROLLER_H

#include "text_field_delegate.h"
#include "zoom_curve_view_controller.h"

namespace Shared {

/* SimpleInteractiveCurveViewController is a View controller with a cursor that
 * can handles zoom in/out and left and right events. */

class SimpleInteractiveCurveViewController : public ZoomCurveViewController, public TextFieldDelegate {
public:
  SimpleInteractiveCurveViewController(Escher::Responder * parentResponder, CurveViewCursor * cursor) : ZoomCurveViewController(parentResponder), m_cursor(cursor) {}

  bool handleEvent(Ion::Events::Event event) override;
  bool textFieldDidReceiveEvent(Escher::AbstractTextField * textField, Ion::Events::Event event) override;

protected:
  constexpr static float k_numberOfCursorStepsInGradUnit = 5.0f;

  // ZoomCurveViewController
  float xFocus() override { return m_cursor->x(); }
  float yFocus() override { return m_cursor->y(); }

  virtual float cursorRightMarginRatio() const { return 0.04f; } // (cursorWidth/2)/(graphViewWidth-1)
  virtual float cursorLeftMarginRatio() const { return 0.04f; }  // (cursorWidth/2)/(graphViewWidth-1)
  virtual float cursorTopMarginRatio() const { return 0.07f; }   // (cursorHeight/2)/(graphViewHeight-1)
  virtual float cursorBottomMarginRatio() const = 0;             // (cursorHeight/2+bannerHeight)/(graphViewHeight-1)
  virtual bool handleLeftRightEvent(Ion::Events::Event event);
  virtual void reloadBannerView() = 0;
  /* the result of moveCursorVertically/Horizontally means:
   * false -> the cursor cannot move in this direction
   * true -> the cursor moved */
  virtual bool moveCursorHorizontally(OMG::NewHorizontalDirection direction, int scrollSpeed = 1) { return false; }

  const AbstractPlotView * constCurveView() const { return const_cast<SimpleInteractiveCurveViewController *>(this)->curveView(); }
  float cursorBottomMarginRatioForBannerHeight(float bannerHeight) const { return (constCurveView()->cursorView()->minimalSizeForOptimalDisplay().height()/2.f + bannerHeight) / (constCurveView()->bounds().height() - 1); }


  CurveViewCursor * m_cursor;
};

}

#endif
