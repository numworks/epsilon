#ifndef GRAPH_GRAPH_CONTROLLER_HELPER_H
#define GRAPH_GRAPH_CONTROLLER_HELPER_H

#include "banner_view.h"
#include "graph_view.h"
#include <apps/shared/curve_view_cursor.h>
#include <apps/shared/function_banner_delegate.h>
#include <apps/shared/interactive_curve_view_range.h>

namespace Graph {

class GraphControllerHelper {
protected:
  bool privateMoveCursorHorizontally(Shared::CurveViewCursor * cursor, OMG::HorizontalDirection direction, Shared::InteractiveCurveViewRange * range, int numberOfStepsInGradUnit, Ion::Storage::Record record, float pixelWidth, int scrollSpeed = 1, int * subCurveIndex = nullptr);
  bool snapToInterestAndUpdateCursor(Shared::CurveViewCursor * cursor, double start, double end, int subCurveIndex);
  // Returns the derivative displayed in banner
  double reloadDerivativeInBannerViewForCursorOnFunction(Shared::CurveViewCursor * cursor, Ion::Storage::Record record);
  virtual BannerView * bannerView() = 0;
  virtual GraphView * graphView() = 0;
  virtual Shared::FunctionBannerDelegate * functionBannerDelegate() = 0;

private:
  constexpr static float k_snapFactor = 1.5f;
  constexpr static double k_definitionDomainDivisor = 96.0;

  virtual void jumpToLeftRightCurve(double t, OMG::HorizontalDirection direction, int functionsCount, Ion::Storage::Record record) {}
};

}

#endif
