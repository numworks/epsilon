#ifndef SHARED_STORAGE_FUNCTION_GRAPH_VIEW_H
#define SHARED_STORAGE_FUNCTION_GRAPH_VIEW_H

#include <escher.h>
#include "curve_view.h"
#include "storage_function.h"
#include "../constant.h"
#include "interactive_curve_view_range.h"

namespace Shared {

class StorageFunctionGraphView : public CurveView {
public:
  StorageFunctionGraphView(InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor,
    BannerView * bannerView, View * cursorView);
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
  char * label(Axis axis, int index) const override;
  char m_xLabels[k_maxNumberOfXLabels][Poincare::PrintFloat::bufferSizeForFloatsWithPrecision(Constant::ShortNumberOfSignificantDigits)];
  char m_yLabels[k_maxNumberOfYLabels][Poincare::PrintFloat::bufferSizeForFloatsWithPrecision(Constant::ShortNumberOfSignificantDigits)];
  Poincare::Context * m_context;
};

}

#endif
