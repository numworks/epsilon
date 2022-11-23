#ifndef INFERENCE_INFERENCE_STATISTIC_TEST_TEST_CURVE_VIEW_H
#define INFERENCE_INFERENCE_STATISTIC_TEST_TEST_CURVE_VIEW_H

#include "inference/models/statistic/test.h"
#include <apps/shared/plot_view_policies.h>

namespace Inference {

class TestPlotPolicy : Shared::PlotPolicy::WithCurves {
public:
  constexpr static int k_zLabelMaxGlyphLength = 4; // "-|z|"

protected:
  void drawPlot(const Shared::AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const;
  void drawZLabelAndZGraduation(const Shared::AbstractPlotView * plotView, KDContext * ctx, KDRect rect, float x, Poincare::ComparisonNode::OperatorType op) const;
  void drawLabelAndGraduation(const Shared::AbstractPlotView * plotView, KDContext * ctx, KDRect rect, float x, Poincare::Layout layout) const;
  void drawTestCurve(const Shared::AbstractPlotView * plotView, KDContext * ctx, KDRect rect, float z, Poincare::ComparisonNode::OperatorType op, double factor = 1.0) const;

  Test * m_test;
};

class TestXAxis : public Shared::PlotPolicy::HorizontalLabeledAxis {
protected:
  void drawLabel(int i, float t, const Shared::AbstractPlotView * plotView, KDContext * ctx, KDRect rect, Shared::AbstractPlotView::Axis axis, KDColor color) const override;
};

typedef Shared::PlotPolicy::Axes<Shared::PlotPolicy::NoGrid, TestXAxis, Shared::PlotPolicy::NoAxis> TestAxes;

class TestCurveView : public Shared::PlotView<TestAxes, TestPlotPolicy, Shared::PlotPolicy::NoBanner, Shared::PlotPolicy::NoCursor> {
public:
  TestCurveView(Test * test);

  Test * test() const { return m_test; }
};

}

#endif
