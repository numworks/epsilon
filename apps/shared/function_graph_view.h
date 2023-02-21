#ifndef SHARED_FUNCTION_GRAPH_VIEW_H
#define SHARED_FUNCTION_GRAPH_VIEW_H

#include <apps/constant.h>

#include "interactive_curve_view_range.h"
#include "plot_view_policies.h"

namespace Shared {

class FunctionGraphPolicy {
 public:
  Poincare::Context *context() const { return m_context; }
  void setContext(Poincare::Context *context) { m_context = context; }

 protected:
  void drawPlot(const AbstractPlotView *plotView, KDContext *ctx,
                KDRect rect) const;

  virtual int numberOfDrawnRecords() const = 0;
  virtual void drawRecord(Ion::Storage::Record record, int index, KDContext *,
                          KDRect, bool firstDrawnRecord) const = 0;
  virtual void tidyModel(int i) const = 0;
  virtual int selectedRecordIndex() const = 0;
  virtual FunctionStore *functionStore() const = 0;

  bool allFunctionsInterrupted() const;
  bool functionWasInterrupted(int index) const;
  void setFunctionInterrupted(int index) const;

  Poincare::Context *m_context;
  mutable uint32_t m_functionsInterrupted;
};

class FunctionGraphView
    : public PlotView<PlotPolicy::TwoLabeledAxes, FunctionGraphPolicy,
                      PlotPolicy::WithBanner, PlotPolicy::WithCursor> {
 public:
  FunctionGraphView(InteractiveCurveViewRange *graphRange,
                    CurveViewCursor *cursor, BannerView *bannerView,
                    CursorView *cursorView);

  // AbstractPlotView
  void drawRect(KDContext *ctx, KDRect rect) const override;
  void reload(bool resetInterrupted = false, bool force = false) override;
  void resetInterruption() override { m_functionsInterrupted = 0; }

  void selectRecord(Ion::Storage::Record record);
  // Select second record to draw area between it and the main record
  void selectSecondRecord(Ion::Storage::Record record);
  virtual void setAreaHighlightColor(bool highlightColor);
  void setAreaHighlight(float start, float end);
  void reloadBetweenBounds(float start, float end);

  Ion::Storage::Record selectedRecord() const { return m_selectedRecord; }
  Ion::Storage::Record secondSelectedRecord() const {
    return m_secondSelectedRecord;
  }

 protected:
  float m_highlightedStart;
  float m_highlightedEnd;
  Ion::Storage::Record m_selectedRecord;
  Ion::Storage::Record m_secondSelectedRecord;
  bool m_shouldColorHighlighted;
};

}  // namespace Shared

#endif
