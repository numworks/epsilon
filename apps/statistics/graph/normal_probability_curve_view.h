#ifndef STATISTICS_NORMAL_PROBABILITY_CURVE_VIEW_H
#define STATISTICS_NORMAL_PROBABILITY_CURVE_VIEW_H

#include <apps/shared/labeled_curve_view.h>
#include <apps/shared/cursor_view.h>
#include <apps/shared/curve_view_cursor.h>
#include "../store.h"

namespace Statistics {

class NormalProbabilityCurveView : public Shared::LabeledCurveView {
public:
  NormalProbabilityCurveView(Shared::CurveViewRange * curveViewRange,
                     Shared::CurveViewCursor * curveViewCursor,
                     Shared::CursorView * cursorView,
                     Store * store);
  void moveCursorTo(int i, int series);
  void drawSeriesCumulatedNormalProbabilityCurve(KDContext * ctx, KDRect rect, int series) const;

  // Escher::View
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  Store * m_store;
};

}  // namespace Statistics

#endif /* STATISTICS_NORMAL_PROBABILITY_CURVE_VIEW_H */
