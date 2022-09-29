#ifndef INFERENCE_INFERENCE_STATISTIC_TEST_TEST_CURVE_VIEW_H
#define INFERENCE_INFERENCE_STATISTIC_TEST_TEST_CURVE_VIEW_H

#include "inference/models/statistic/test.h"
#include <apps/shared/plot_view_policies.h>

namespace Inference {

class TestPlotPolicy : Shared::PlotPolicy::WithCurves {
protected:
  void drawPlot(const Shared::AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const;
  void drawZLabelAndZGraduation(const Shared::AbstractPlotView * plotView, KDContext * ctx, KDRect rect, float x, HypothesisParams::ComparisonOperator op) const;
  void drawLabelAndGraduation(const Shared::AbstractPlotView * plotView, KDContext * ctx, KDRect rect, float x, Poincare::Layout layout) const;
  void drawTestCurve(const Shared::AbstractPlotView * plotView, KDContext * ctx, KDRect rect, float z, HypothesisParams::ComparisonOperator op, double factor = 1.0) const;

  Test * m_test;
};

class TestCurveView : public Shared::PlotView<Shared::PlotPolicy::LabeledXAxis, TestPlotPolicy, Shared::PlotPolicy::NoBanner, Shared::PlotPolicy::NoCursor> {
public:
  TestCurveView(Test * test);
};

}

#endif
