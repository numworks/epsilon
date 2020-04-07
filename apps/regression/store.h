#ifndef REGRESSION_STORE_H
#define REGRESSION_STORE_H

#include "model/model.h"
#include "model/cubic_model.h"
#include "model/exponential_model.h"
#include "model/linear_model.h"
#include "model/logarithmic_model.h"
#include "model/logistic_model.h"
#include "model/power_model.h"
#include "model/quadratic_model.h"
#include "model/quartic_model.h"
#include "model/trigonometric_model.h"
#include "../shared/interactive_curve_view_range.h"
#include "../shared/double_pair_store.h"
#include <escher/responder.h>
#include <float.h>

namespace Regression {

class Store : public Shared::InteractiveCurveViewRange, public Shared::DoublePairStore {
public:
  Store();

  void reset();
  // Clean pool
  void tidy();

  // Regression
  void setSeriesRegressionType(int series, Model::Type type);
  Model::Type seriesRegressionType(int series) {
    return m_regressionTypes[series];
  }
  Model * modelForSeries(int series) {
    assert(series >= 0 && series < k_numberOfSeries);
    assert((int)m_regressionTypes[series] >= 0 && (int)m_regressionTypes[series] < Model::k_numberOfModels);
    return regressionModel((int)m_regressionTypes[series]);
  }
  uint32_t * seriesChecksum() { return m_seriesChecksum; }

  // Dots
  /* Return the closest dot to abscissa x above the regression curve if
   * direction > 0, below otherwise */
  int closestVerticalDot(int direction, double x, double y, int currentSeries, int currentDot, int * nextSeries, Poincare::Context * globalContext);
  /* Return the closest dot to given dot, on the right if direction > 0,
   * on the left otherwise */
  int nextDot(int series, int direction, int dot);
  Model * regressionModel(Model::Type type) {
    return regressionModel((int) type);
  }

  // Window
  void setDefault() override;

  // Series
  bool seriesIsEmpty(int series) const override;

  // Calculation
  double * coefficientsForSeries(int series, Poincare::Context * globalContext);
  double doubleCastedNumberOfPairsOfSeries(int series) const;
  double squaredValueSumOfColumn(int series, int i, bool lnOfSeries = false) const;
  double columnProductSum(int series, bool lnOfSeries = false) const;
  double meanOfColumn(int series, int i, bool lnOfSeries = false) const;
  double varianceOfColumn(int series, int i, bool lnOfSeries = false) const;
  double standardDeviationOfColumn(int series, int i, bool lnOfSeries = false) const;
  double covariance(int series, bool lnOfSeries = false) const;
  double slope(int series, bool lnOfSeries = false) const;
  double yIntercept(int series, bool lnOfSeries = false) const;
  double yValueForXValue(int series, double x, Poincare::Context * globalContext);
  double xValueForYValue(int series, double y, Poincare::Context * globalContext);
  double correlationCoefficient(int series) const;
  double squaredCorrelationCoefficient(int series) const;
private:
  constexpr static float k_displayHorizontalMarginRatio = 0.05f;
  void resetMemoization();
  float maxValueOfColumn(int series, int i) const; //TODO LEA why float ?
  float minValueOfColumn(int series, int i) const; //TODO LEA why float ?
  Model * regressionModel(int index);
  uint32_t m_seriesChecksum[k_numberOfSeries];
  Model::Type m_regressionTypes[k_numberOfSeries];
  LinearModel m_linearModel;
  QuadraticModel m_quadraticModel;
  CubicModel m_cubicModel;
  QuarticModel m_quarticModel;
  LogarithmicModel m_logarithmicModel;
  ExponentialModel m_exponentialModel;
  PowerModel m_powerModel;
  TrigonometricModel m_trigonometricModel;
  LogisticModel m_logisticModel;
  double m_regressionCoefficients[k_numberOfSeries][Model::k_maxNumberOfCoefficients];
  bool m_regressionChanged[k_numberOfSeries];
  Poincare::Preferences::AngleUnit m_angleUnit;
};

typedef double (Store::*ArgCalculPointer)(int, int, bool) const;
typedef void (Store::*RangeMethodPointer)();

}

#endif
