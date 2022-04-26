#ifndef STATISTICS_CURVE_VIEW_CURSOR_H
#define STATISTICS_CURVE_VIEW_CURSOR_H

#include <apps/shared/curve_view_cursor.h>

namespace Statistics {

class PlotCursor : public Shared::CurveViewCursor {
public:
  using Shared::CurveViewCursor::CurveViewCursor;
  void moveTo(double t, double x, double y) override;
};

}

#endif
