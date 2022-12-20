#ifndef SHARED_ZOOM_AND_PAN_CURVE_VIEW_CONTROLLER_H
#define SHARED_ZOOM_AND_PAN_CURVE_VIEW_CONTROLLER_H

#include "zoom_curve_view_controller.h"

namespace Shared {

/* ZoomAndPanCurveViewController is a View controller with a cursor that can
 * handles zoom in/out and directional pan events. */

class ZoomAndPanCurveViewController : public ZoomCurveViewController {
public:
  ZoomAndPanCurveViewController(Responder * parentResponder) : ZoomCurveViewController(parentResponder) {}
  bool handleEvent(Ion::Events::Event & event) override;
protected:
  virtual bool handlePan(Ion::Events::Event event);
  float xFocus() override { return interactiveCurveViewRange()->xCenter(); }
  float yFocus() override { return interactiveCurveViewRange()->yCenter(); }
};

}

#endif
