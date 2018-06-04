#ifndef REGRESSION_STORE_H
#define REGRESSION_STORE_H

#include "../shared/interactive_curve_view_range.h"
#include "../shared/double_pair_store.h"
extern "C" {
#include <float.h>
}

namespace Regression {

class Store : public Shared::InteractiveCurveViewRange, public Shared::DoublePairStore, public Shared::InteractiveCurveViewRangeDelegate {
public:
  Store();

  // Regression
  /* Return the series index of the closest regression at abscissa x, above
   * ordinate y if direction > 0, below otherwise */
  int closestVerticalRegression(int direction, float x, float y, int currentRegressionSeries);
  // Dots
  /* Return the closest dot to abscissa x above the regression curve if
   * direction > 0, below otherwise */
  int closestVerticalDot(int direction, float x, float y, int currentSeries, int currentDot, int * nextSeries);
  /* Return the closest dot to given dot, on the right if direction > 0,
   * on the left otherwise */
  int nextDot(int series, int direction, int dot);

  // Window
  void setDefault() override;

  // Series
  bool isEmpty() const;
  int numberOfNonEmptySeries() const;
  bool seriesIsEmpty(int series) const;
  int indexOfKthNonEmptySeries(int k) const;

  // Calculation
  double doubleCastedNumberOfPairsOfSeries(int series) const;
  double squaredValueSumOfColumn(int series, int i) const;
  double columnProductSum(int series) const;
  double meanOfColumn(int series, int i) const;
  double varianceOfColumn(int series, int i) const;
  double standardDeviationOfColumn(int series, int i) const;
  double covariance(int series) const;
  double slope(int series) const;
  double yIntercept(int series) const;
  double yValueForXValue(int series, double x) const;
  double xValueForYValue(int series, double y) const;
  double correlationCoefficient(int series) const;
  double squaredCorrelationCoefficient(int series) const;
private:
  constexpr static float k_displayTopMarginRatio = 0.12f;
  constexpr static float k_displayRightMarginRatio = 0.05f;
  constexpr static float k_displayBottomMarginRatio = 0.5f;
  constexpr static float k_displayLeftMarginRatio = 0.05f;
  InteractiveCurveViewRangeDelegate::Range computeYRange(InteractiveCurveViewRange * interactiveCurveViewRange) override;
  float addMargin(float x, float range, bool isMin) override;
  float maxValueOfColumn(int series, int i) const;
  float minValueOfColumn(int series, int i) const;
};

typedef double (Store::*ArgCalculPointer)(int, int) const;
typedef double (Store::*CalculPointer)(int) const;
typedef void (Store::*RangeMethodPointer)();

}

#endif
