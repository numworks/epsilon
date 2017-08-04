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
  int closestVerticalDot(int direction, float x);
  /* Return the closest dot to dot given on the right if direction > 0,
   * on the left otherwise*/
  int nextDot(int direction, int dot);

  // Window
  void setDefault() override;

  // Calculation
  double numberOfPairs();
  double squaredValueSumOfColumn(int i);
  double columnProductSum();
  double meanOfColumn(int i);
  double varianceOfColumn(int i);
  double standardDeviationOfColumn(int i);
  double covariance();
  double slope();
  double yIntercept();
  double yValueForXValue(double x);
  double xValueForYValue(double y);
  double correlationCoefficient();
  double squaredCorrelationCoefficient();
private:
  constexpr static float k_displayTopMarginRatio = 0.12f;
  constexpr static float k_displayRightMarginRatio = 0.05f;
  constexpr static float k_displayBottomMarginRatio = 0.5f;
  constexpr static float k_displayLeftMarginRatio = 0.05f;
  InteractiveCurveViewRangeDelegate::Range computeYRange(InteractiveCurveViewRange * interactiveCurveViewRange) override;
  float addMargin(float x, float range, bool isMin) override;
  float maxValueOfColumn(int i);
  float minValueOfColumn(int i);
};

typedef double (Store::*ArgCalculPointer)(int);
typedef double (Store::*CalculPointer)();
typedef void (Store::*RangeMethodPointer)();

}

#endif
