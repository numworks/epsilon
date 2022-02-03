#ifndef PROBABILITY_GUI_STATISTIC_CURVE_VIEW_H
#define PROBABILITY_GUI_STATISTIC_CURVE_VIEW_H

#include <apps/shared/curve_view.h>
#include <escher/palette.h>
#include <poincare/coordinate_2D.h>
#include <poincare/layout.h>

#include "probability/models/statistic/statistic.h"

namespace Probability {

class StatisticCurveView : public Shared::CurveView {
public:
  using Shared::CurveView::CurveView;
  void setStatistic(Statistic * statistic) { m_statistic = statistic; }
  void drawRect(KDContext * ctx, KDRect rect) const override;

protected:
  char * label(Axis axis, int index) const override;
  bool shouldDrawLabelAtPosition(float labelValue) const override;

private:
  constexpr static int k_marginsAroundZLabel = 30;
  constexpr static int k_numberOfIntervals = 4;
  constexpr static KDCoordinate k_intervalThickness = 1;
  constexpr static KDCoordinate k_mainIntervalBoundHeight = 9;

  static int IntervalMainThresholdIndex(float mainThreshold);
  static float NextThreshold(float threshold, bool up);
  static float IntervalThresholdAtIndex(float mainThreshold, int index);

  void drawTest(KDContext * ctx, KDRect rect) const;
  void drawInterval(KDContext * ctx, KDRect rect) const;
  void colorUnderCurve(KDContext * ctx,
                       KDRect rect,
                       HypothesisParams::ComparisonOperator op,
                       float z) const;
  void drawLabelAndGraduationAtPosition(KDContext * ctx, float position, const char * text) const;
  void drawLabelAndGraduationAtPosition(KDContext * ctx, float position, Poincare::Layout symbol) const;
  void drawZLabelAndZGraduation(KDContext * ctx, float x) const;
  void drawIntervalLabelAndGraduation(KDContext * ctx) const;
  KDCoordinate drawGraduationAtPosition(KDContext * ctx, float horizontalPosition, float verticalPosition = 0.0f, KDColor color = KDColorBlack, KDCoordinate length = k_labelGraduationLength, KDCoordinate axisThickness = 2) const;

  void convertFloatToText(float value, char * buffer, int bufferSize) const;

  static Poincare::Coordinate2D<float> evaluateTestAtAbscissa(float x,
                                                              void * model,
                                                              void * context);
  static Poincare::Coordinate2D<float> evaluateIntervalAtAbscissa(float x,
                                                                  void * model,
                                                                  void * context);
  static constexpr KDColor k_backgroundColor = Escher::Palette::WallScreen;
  char m_labels[k_maxNumberOfXLabels][k_labelBufferMaxSize];
  Statistic * m_statistic;
};

}  // namespace Probability

#endif /* PROBABILITY_GUI_STATISTIC_CURVE_VIEW_H */
