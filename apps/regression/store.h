#ifndef REGRESSION_STORE_H
#define REGRESSION_STORE_H

#include "../shared/interactive_curve_view_range.h"
#include "../shared/float_pair_store.h"

namespace Regression {

class Store : public Shared::InteractiveCurveViewRange, public Shared::FloatPairStore, public Shared::InteractiveCurveViewRangeDelegate {
public:
  Store();
  // Dots
  /* Return the closest dot to x above the regression curve if direction > 0,
   * below otherwise*/
  int closestVerticalDot(int series, int direction, float x);
  /* Return the closest dot to dot given on the right if direction > 0,
   * on the left otherwise*/
  int nextDot(int series, int direction, int dot);

  // Window
  void setDefault() override;

  // Calculation
  double numberOfPairs(int series) const { return m_numberOfPairs; }
  double squaredValueSumOfColumn(int series, int i);
  double columnProductSum(int series);
  double meanOfColumn(int series, int i);
  double varianceOfColumn(int series, int i);
  double standardDeviationOfColumn(int series, int i);
  double covariance(int series);
  double slope(int series);
  double yIntercept(int series);
  double yValueForXValue(int series, double x);
  double xValueForYValue(int series, double y);
  double correlationCoefficient(int series);
  double squaredCorrelationCoefficient(int series);
private:
  constexpr static float k_displayTopMarginRatio = 0.12f;
  constexpr static float k_displayRightMarginRatio = 0.05f;
  constexpr static float k_displayBottomMarginRatio = 0.5f;
  constexpr static float k_displayLeftMarginRatio = 0.05f;
  InteractiveCurveViewRangeDelegate::Range computeYRange(int series, InteractiveCurveViewRange * interactiveCurveViewRange) override;
  float addMargin(float x, float range, bool isMin) override;
  float maxValueOfColumn(int series, int i);
  float minValueOfColumn(int series, int i);
};

typedef double (Store::*ArgCalculPointer)(int);
typedef double (Store::*CalculPointer)();
typedef void (Store::*RangeMethodPointer)();

}

#endif
