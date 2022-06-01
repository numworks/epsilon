#ifndef INFERENCE_INFERENCE_STATISTIC_TEST_TEST_CURVE_VIEW_H
#define INFERENCE_INFERENCE_STATISTIC_TEST_TEST_CURVE_VIEW_H

#include "inference/models/statistic/test.h"
#include "inference/statistic/statistic_curve_view.h"

namespace Inference {

class TestCurveView : public StatisticCurveView {
public:
  TestCurveView(Test * test) : StatisticCurveView(test), m_test(test) {}
  void drawRect(KDContext * ctx, KDRect rect) const override;

protected:
  bool shouldDrawLabelAtPosition(float labelValue) const override;

private:
  constexpr static int k_marginsAroundZLabel = 30;
  void drawTest(KDContext * ctx, KDRect rect) const;
  void drawAlphaStripes(KDContext * ctx, KDRect rect, HypothesisParams::ComparisonOperator op, float z) const;
  void colorUnderCurve(KDContext * ctx, KDRect rect, HypothesisParams::ComparisonOperator op, float z) const;
  void drawLabelAndGraduationAtPosition(KDContext * ctx, float position, Poincare::Layout symbol) const;
  void drawZLabelAndZGraduation(KDContext * ctx, float x) const;

  static Poincare::Coordinate2D<float> evaluateAtAbscissa(float x, void * model, void * context);

  Test * m_test;
};

}  // namespace Inference

#endif /* INFERENCE_INFERENCE_STATISTIC_TEST_TEST_CURVE_VIEW_H */
