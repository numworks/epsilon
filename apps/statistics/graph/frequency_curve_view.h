#ifndef STATISTICS_FREQUENCY_CURVE_VIEW_H
#define STATISTICS_FREQUENCY_CURVE_VIEW_H

#include <apps/shared/labeled_curve_view.h>
#include <apps/shared/cursor_view.h>
#include <apps/shared/curve_view_cursor.h>
#include "../store.h"

namespace Statistics {

class FrequencyCurveView : public Shared::LabeledCurveView {
public:
  FrequencyCurveView(Shared::CurveViewRange * curveViewRange,
                     Shared::CurveViewCursor * curveViewCursor,
                     Shared::CursorView * cursorView,
                     Store * store);
  void moveCursorTo(int i, int series);
  void drawSeriesCumulatedFrequencyCurve(KDContext * ctx, KDRect rect, int series) const;

  // Shared::LabeledCurveView
  // Append '%' to vertical axis labels.
  void appendLabelSuffix(Axis axis, char * labelBuffer, int maxSize, int glyphLength, int maxGlyphLength) override;

  // Escher::View
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  Store * m_store;
};

}  // namespace Statistics

#endif /* STATISTICS_FREQUENCY_CURVE_VIEW_H */
