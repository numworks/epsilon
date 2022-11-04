#ifndef GRAPH_GRAPH_CONTROLLER_HELPER_H
#define GRAPH_GRAPH_CONTROLLER_HELPER_H

#include "banner_view.h"
#include <apps/shared/curve_view_cursor.h>
#include <apps/shared/memoized_cursor_view.h>
#include <apps/shared/interactive_curve_view_range.h>

namespace Graph {

class GraphControllerHelper {
protected:
  bool privateMoveCursorHorizontally(Shared::CurveViewCursor * cursor, int direction, Shared::InteractiveCurveViewRange * range, int numberOfStepsInGradUnit, Ion::Storage::Record record, float pixelWidth, int scrollSpeed = 1, int * subCurveIndex = nullptr);
  bool snapToInterestAndUpdateBanner(double * t, double start, double end);
  void reloadDerivativeInBannerViewForCursorOnFunction(Shared::CurveViewCursor * cursor, Ion::Storage::Record record);
  virtual BannerView * bannerView() = 0;
  virtual Shared::MemoizedCursorView * cursorView() { return nullptr; }

private:
  constexpr static float k_snapFactor = 1.5f;
  constexpr static double k_definitionDomainDivisor = 96.0;

  virtual void jumpToLeftRightCurve(double t, int direction, int functionsCount, Ion::Storage::Record record) {}
};

}

#endif
