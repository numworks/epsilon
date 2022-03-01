#ifndef STATISTICS_PLOT_CURVE_VIEW_H
#define STATISTICS_PLOT_CURVE_VIEW_H

#include <apps/shared/labeled_curve_view.h>
#include <apps/shared/cursor_view.h>
#include <apps/shared/curve_view_cursor.h>
#include "../store.h"

namespace Statistics {

class PlotCurveView : public Shared::LabeledCurveView {
public:
  PlotCurveView(Shared::CurveViewRange * curveViewRange,
                Shared::CurveViewCursor * curveViewCursor,
                Shared::CursorView * cursorView,
                Store * store);
  void moveCursorTo(int i, int series);
  virtual double valueAtIndex(int series, int * sortedIndex, int i) const = 0;
  virtual void drawSeriesCurve(KDContext * ctx, KDRect rect, int series) const;

  // Escher::View
  void drawRect(KDContext * ctx, KDRect rect) const override;
protected:
  virtual bool connectPoints() const { return false; }

  Store * m_store;
};

}  // namespace Statistics

#endif /* STATISTICS_PLOT_CURVE_VIEW_H */
