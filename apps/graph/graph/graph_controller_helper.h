#ifndef GRAPH_GRAPH_CONTROLLER_HELPER_H
#define GRAPH_GRAPH_CONTROLLER_HELPER_H

#include "banner_view.h"
#include "../../shared/curve_view_cursor.h"
#include "../../shared/interactive_curve_view_range.h"

namespace Graph {

class App;

class GraphControllerHelper {
protected:
  bool privateMoveCursorHorizontally(Shared::CurveViewCursor * cursor, int direction, Shared::InteractiveCurveViewRange * range, int numberOfStepsInGradUnit, Ion::Storage::Record record, int scrollSpeed = 1, int * subCurveIndex = nullptr);
  void reloadDerivativeInBannerViewForCursorOnFunction(Shared::CurveViewCursor * cursor, Ion::Storage::Record record);
  virtual BannerView * bannerView() = 0;
private:
  constexpr static double k_definitionDomainDivisor = 96.0;
  constexpr static double k_minimalSlopeMultiplicator = 0.1;

  virtual void jumpToLeftRightCurve(double t, int direction, int functionsCount, Ion::Storage::Record record) {}
};

}

#endif
