#ifndef SHARED_STORAGE_FUNCTION_GRAPH_VIEW_H
#define SHARED_STORAGE_FUNCTION_GRAPH_VIEW_H

#include <escher.h>
#include "curve_view.h"
#include "../constant.h"
#include "interactive_curve_view_range.h"

namespace Shared {

template <class T>
class StorageFunctionGraphView : public CurveView {
public:
  StorageFunctionGraphView(InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor, BannerView * bannerView, View * cursorView) :
    CurveView(graphRange, cursor, bannerView, cursorView),
    m_selectedFunction(Ion::Storage::Record()),
    m_highlightedStart(NAN),
    m_highlightedEnd(NAN),
    m_shouldColorHighlighted(false),
    m_xLabels{},
    m_yLabels{},
    m_context(nullptr)
    {}
  void drawRect(KDContext * ctx, KDRect rect) const override {
    ctx->fillRect(rect, KDColorWhite);
    drawGrid(ctx, rect);
    drawAxes(ctx, rect, Axis::Horizontal);
    drawAxes(ctx, rect, Axis::Vertical);
    drawLabels(ctx, rect, Axis::Horizontal, true);
    drawLabels(ctx, rect, Axis::Vertical, true);
  }
  void setContext(Poincare::Context * context) {
    m_context = context;
  }
  Poincare::Context * context() const { return m_context; }
  void selectFunction(T * function) {
    if (m_selectedFunction != *function) {
      m_selectedFunction = *function;
      reloadBetweenBounds(m_highlightedStart, m_highlightedEnd);
    }
  }
  void setAreaHighlight(float start, float end) {
    if (m_highlightedStart != start || m_highlightedEnd != end) {
      float dirtyStart = start > m_highlightedStart ? m_highlightedStart : start;
      float dirtyEnd = start > m_highlightedStart ? start : m_highlightedStart;
      reloadBetweenBounds(dirtyStart, dirtyEnd);
      dirtyStart = end > m_highlightedEnd ? m_highlightedEnd : end;
      dirtyEnd = end > m_highlightedEnd ? end : m_highlightedEnd;
      reloadBetweenBounds(dirtyStart, dirtyEnd);
      if (std::isnan(start) || std::isnan(end)) {
        reloadBetweenBounds(m_highlightedStart, m_highlightedEnd);
      }
      if (std::isnan(m_highlightedStart) || std::isnan(m_highlightedEnd)) {
        reloadBetweenBounds(start, end);
      }
      m_highlightedStart = start;
      m_highlightedEnd = end;
    }
  }
  virtual void setAreaHighlightColor(bool highlightColor)  {
    if (m_shouldColorHighlighted != highlightColor) {
      reloadBetweenBounds(m_highlightedStart, m_highlightedEnd);
      m_shouldColorHighlighted = highlightColor;
    }
  }
protected:
  void reloadBetweenBounds(float start, float end) {
    if (start == end) {
      return;
    }
    float pixelLowerBound = floatToPixel(Axis::Horizontal, start)-2.0;
    float pixelUpperBound = floatToPixel(Axis::Horizontal, end)+4.0;
    /* We exclude the banner frame from the dirty zone to avoid unnecessary
     * redrawing */
    KDRect dirtyZone(KDRect(pixelLowerBound, 0, pixelUpperBound-pixelLowerBound,
          bounds().height()-m_bannerView->bounds().height()));
    markRectAsDirty(dirtyZone);
  }
  T m_selectedFunction;
  float m_highlightedStart;
  float m_highlightedEnd;
  bool m_shouldColorHighlighted;
private:
  char * label(Axis axis, int index) const override {
    return (axis == Axis::Horizontal ? (char *)m_xLabels[index] : (char *)m_yLabels[index]);
  }
  char m_xLabels[k_maxNumberOfXLabels][Poincare::PrintFloat::bufferSizeForFloatsWithPrecision(Constant::ShortNumberOfSignificantDigits)];
  char m_yLabels[k_maxNumberOfYLabels][Poincare::PrintFloat::bufferSizeForFloatsWithPrecision(Constant::ShortNumberOfSignificantDigits)];
  Poincare::Context * m_context;
};

}

#endif
