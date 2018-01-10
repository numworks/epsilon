#ifndef SHARED_FUNCTION_GRAPH_VIEW_H
#define SHARED_FUNCTION_GRAPH_VIEW_H

#include <escher.h>
#include "curve_view.h"
#include "function.h"
#include "../constant.h"
#include "interactive_curve_view_range.h"

namespace Shared {

class FunctionGraphView : public CurveView {
public:
  FunctionGraphView(InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor,
    BannerView * bannerView, View * cursorView);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setContext(Poincare::Context * context);
  Poincare::Context * context() const;
  void selectFunction(Function * function);
  void setVerticalCursor(bool verticalCursor);
  void setAreaHighlight(float start, float end);
  virtual void setAreaHighlightColor(bool highlightColor);
protected:
  void reloadBetweenBounds(float start, float end);
  Function * m_selectedFunction;
  bool m_verticalCursor;
  float m_highlightedStart;
  float m_highlightedEnd;
  bool m_shouldColorHighlighted;
private:
  KDSize cursorSize() override;
  char * label(Axis axis, int index) const override;
  char m_xLabels[k_maxNumberOfXLabels][Poincare::PrintFloat::bufferSizeForFloatsWithPrecision(Constant::ShortNumberOfSignificantDigits)];
  char m_yLabels[k_maxNumberOfYLabels][Poincare::PrintFloat::bufferSizeForFloatsWithPrecision(Constant::ShortNumberOfSignificantDigits)];
  Poincare::Context * m_context;
};

}

#endif
