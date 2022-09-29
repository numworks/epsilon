#ifndef SHARED_FUNCTION_GRAPH_VIEW_H
#define SHARED_FUNCTION_GRAPH_VIEW_H

#include "labeled_curve_view.h"
#include "../constant.h"
#include "interactive_curve_view_range.h"

namespace Shared {

class FunctionGraphView : public LabeledCurveView {
public:
  FunctionGraphView(InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor,
    BannerView * bannerView, CursorView * cursorView);
  void reload(bool resetInterrupted = false, bool force = false) override;
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setContext(Poincare::Context * context);
  Poincare::Context * context() const;
  void selectRecord(Ion::Storage::Record record);
  // Select second record to draw area between it and the main record
  void selectSecondRecord(Ion::Storage::Record record);
  void setAreaHighlight(float start, float end);
  virtual void setAreaHighlightColor(bool highlightColor);
  void resetCurvesInterrupted() override { m_functionsInterrupted = 0; }

  Ion::Storage::Record selectedRecord() const { return m_selectedRecord; }
  Ion::Storage::Record secondSelectedRecord() const { return m_secondSelectedRecord; }

protected:
  void reloadBetweenBounds(float start, float end);
  Ion::Storage::Record m_selectedRecord;
  Ion::Storage::Record m_secondSelectedRecord;
  float m_highlightedStart;
  float m_highlightedEnd;
  bool m_shouldColorHighlighted;
  bool allFunctionsInterrupted(int numberOfFunctions) const;
  bool functionWasInterrupted(int index) const;
  void setFunctionInterrupted(int index) const;

  mutable uint32_t m_functionsInterrupted;
  Poincare::Context * m_context;
};

}

#endif
