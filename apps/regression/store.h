#ifndef REGRESSION_STORE_H
#define REGRESSION_STORE_H

#include "../interactive_curve_view_range.h"
#include "../float_pair_store.h"

namespace Regression {

class Store : public InteractiveCurveViewRange, public FloatPairStore, public InteractiveCurveViewRangeDelegate {
public:
  Store();
  bool didChangeRange(InteractiveCurveViewRange * interactiveCurveViewRange) override;

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
  float squaredValueSumOfColumn(int i);
  float columnProductSum();
  float meanOfColumn(int i);
  float varianceOfColumn(int i);
  float standardDeviationOfColumn(int i);
  float covariance();
  float slope();
  float yIntercept();
  float yValueForXValue(float x);
  float xValueForYValue(float y);
  float correlationCoefficient();
  float squaredCorrelationCoefficient();
private:
  float maxValueOfColumn(int i);
  float minValueOfColumn(int i);
};

}

#endif
