#ifndef REGRESSION_STORE_H
#define REGRESSION_STORE_H

#include "model/model.h"
#include "../shared/interactive_curve_view_range.h"
#include "../shared/double_pair_store.h"
#include <escher/responder.h>
#include <float.h>

namespace Regression {

class Store : public Shared::InteractiveCurveViewRange, public Shared::DoublePairStore, public Shared::InteractiveCurveViewRangeDelegate {
public:
  Store();
  ~Store();
  Store(const Store & other) = delete;
  Store(Store && other) = delete;
  Store& operator=(const Store & other) = delete;
  Store& operator=(Store && other) = delete;

  // Regression
  void setSeriesRegressionType(int series, Model::Type type);
  Model::Type seriesRegressionType(int series) {
    return m_regressionTypes[series];
  }
  Model * modelForSeries(int series) {
    assert(series >= 0 && series < k_numberOfSeries);
    assert((int)m_regressionTypes[series] >= 0 && (int)m_regressionTypes[series] < Model::k_numberOfModels);
    return m_regressionModels[(int)m_regressionTypes[series]];
  }
  /* Return the series index of the closest regression at abscissa x, above
   * ordinate y if direction > 0, below otherwise */
  int closestVerticalRegression(int direction, float x, float y, int currentRegressionSeries, Poincare::Context * globalContext);
  // Dots
  /* Return the closest dot to abscissa x above the regression curve if
   * direction > 0, below otherwise */
  int closestVerticalDot(int direction, float x, float y, int currentSeries, int currentDot, int * nextSeries, Poincare::Context * globalContext);
  /* Return the closest dot to given dot, on the right if direction > 0,
   * on the left otherwise */
  int nextDot(int series, int direction, int dot);
  Model * regressionModel(Model::Type type) {
    return m_regressionModels[(int) type];
  }

  // Window
  void setDefault() override;

  // Series
  bool seriesIsEmpty(int series) const override;

  // Calculation
  double * coefficientsForSeries(int series, Poincare::Context * globalContext);
  double doubleCastedNumberOfPairsOfSeries(int series) const;
  double squaredValueSumOfColumn(int series, int i) const;
  double columnProductSum(int series) const;
  double meanOfColumn(int series, int i) const;
  double varianceOfColumn(int series, int i) const;
  double standardDeviationOfColumn(int series, int i) const;
  double covariance(int series) const;
  double slope(int series) const;
  double yIntercept(int series) const;
  double yValueForXValue(int series, double x, Poincare::Context * globalContext);
  double xValueForYValue(int series, double y, Poincare::Context * globalContext);
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
  uint32_t m_seriesChecksum[k_numberOfSeries];
  Model::Type m_regressionTypes[k_numberOfSeries];
  Model * m_regressionModels[Model::k_numberOfModels];
  double m_regressionCoefficients[k_numberOfSeries][Model::k_maxNumberOfCoefficients];
  bool m_regressionChanged[k_numberOfSeries];
};

typedef double (Store::*ArgCalculPointer)(int, int) const;
typedef double (Store::*CalculPointer)(int) const;
typedef void (Store::*RangeMethodPointer)();

}

#endif
