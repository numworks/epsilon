#ifndef SHARED_ZOOM_CURVE_VIEW_CONTROLLER_H
#define SHARED_ZOOM_CURVE_VIEW_CONTROLLER_H

#include <escher/view_controller.h>
#include "interactive_curve_view_range.h"
#include "curve_view_cursor.h"
#include "curve_view.h"

namespace Shared {

/* ZoomCurveViewController is a View controller with a cursor that can handles
 * zoom in/out events. */

class ZoomCurveViewController : public ViewController {
public:
  ZoomCurveViewController(Responder * parentResponder) : ViewController(parentResponder) {}
  View * view() override { return curveView(); }
  bool handleEvent(Ion::Events::Event event) override;
protected:
  virtual bool handleZoom(Ion::Events::Event event);
  virtual InteractiveCurveViewRange * interactiveCurveViewRange() = 0;
  virtual CurveView * curveView() = 0;
  virtual float xFocus() = 0;
  virtual float yFocus() = 0;
  CurveViewCursor * m_cursor;
};

}

#endif
