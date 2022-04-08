#ifndef STATISTICS_PLOT_CURVE_VIEW_H
#define STATISTICS_PLOT_CURVE_VIEW_H

#include <apps/shared/labeled_curve_view.h>
#include <apps/shared/cursor_view.h>
#include <apps/shared/curve_view_cursor.h>
#include "../store.h"

namespace Statistics {

class PlotControllerDelegate;

class PlotCurveView : public Shared::LabeledCurveView {
public:
  PlotCurveView(Shared::CurveViewRange * curveViewRange,
                Shared::CurveViewCursor * curveViewCursor,
                Shared::CursorView * cursorView,
                PlotControllerDelegate * plotControllerDelegate);
  void drawSeriesCurve(KDContext * ctx, KDRect rect, int series) const;

  // Escher::View
  void drawRect(KDContext * ctx, KDRect rect) const override;

  // Shared::LabeledCurveView
  void appendLabelSuffix(Axis axis, char * labelBuffer, int maxSize, int glyphLength, int maxGlyphLength) override;
private:
  float labelStepFactor(Axis axis) const override;

  PlotControllerDelegate * m_plotControllerDelegate;
};

}  // namespace Statistics

#endif /* STATISTICS_PLOT_CURVE_VIEW_H */
