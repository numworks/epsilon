#ifndef SHARED_FUNCTION_GRAPH_VIEW_H
#define SHARED_FUNCTION_GRAPH_VIEW_H

#include <escher.h>
#include "labeled_curve_view.h"
#include "function.h"
#include "../constant.h"
#include "interactive_curve_view_range.h"

namespace Shared {

class FunctionGraphView : public LabeledCurveView {
public:
  FunctionGraphView(InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor,
    BannerView * bannerView, CursorView * cursorView);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setContext(Poincare::Context * context);
  Poincare::Context * context() const;
  void selectRecord(Ion::Storage::Record record);
  void setAreaHighlight(float start, float end);
  virtual void setAreaHighlightColor(bool highlightColor);
protected:
  void reloadBetweenBounds(float start, float end);
  Ion::Storage::Record m_selectedRecord;
  float m_highlightedStart;
  float m_highlightedEnd;
  bool m_shouldColorHighlighted;
private:
  Poincare::Context * m_context;
};

}

#endif
