#ifndef REGRESSION_STORE_H
#define REGRESSION_STORE_H

#include "model/model.h"
#include "model/cubic_model.h"
#include "model/exponential_model.h"
#include "model/linear_model.h"
#include "model/logarithmic_model.h"
#include "model/logistic_model.h"
#include "model/none_model.h"
#include "model/power_model.h"
#include "model/proportional_model.h"
#include "model/quadratic_model.h"
#include "model/quartic_model.h"
#include "model/trigonometric_model.h"
#include "model/median_model.h"
#include "../shared/interactive_curve_view_range.h"
#include "../shared/linear_regression_store.h"
#include <escher/responder.h>
#include <float.h>

namespace Regression {

class Store : public Shared::InteractiveCurveViewRange, public Shared::LinearRegressionStore {
public:
  constexpr static const char * const * k_columnNames = DoublePairStore::k_regressionColumNames;
  static const char * SeriesTitle(int series);

  Store(Shared::GlobalContext * context, Model::Type * regressionTypes);

  void reset();

  // Regression
  void setSeriesRegressionType(int series, Model::Type type);
  Model::Type seriesRegressionType(int series) {
    return m_regressionTypes[series];
  }
  Model * modelForSeries(int series) {
    assert(series >= 0 && series < k_numberOfSeries);
    assert((int)m_regressionTypes[series] >= 0 && (int)m_regressionTypes[series] < Model::k_numberOfModels);
    return regressionModel(m_regressionTypes[series]);
  }
  uint32_t * seriesChecksum() { return m_seriesChecksum; }

  // Dots
  /* Return the closest dot to abscissa x above the regression curve if
   * direction > 0, below otherwise */
  int closestVerticalDot(int direction, double x, double y, int currentSeries, int currentDot, int * nextSeries, Poincare::Context * globalContext);
  /* Return the closest dot to given dot, on the right if direction > 0,
   * on the left otherwise */
  int nextDot(int series, int direction, int dot, bool displayMean);
  Model * regressionModel(Model::Type type) {
    return regressionModel((int) type);
  }

  // Series
  void updateSeriesValidity(int series) override;

  // Calculation
  void updateCoefficients(int series, Poincare::Context * globalContext);
  double * coefficientsForSeries(int series, Poincare::Context * globalContext);
  bool coefficientsAreDefined(int series, Poincare::Context * globalContext);
  double determinationCoefficientForSeries(int series, Poincare::Context * globalContext); // R2
  double yValueForXValue(int series, double x, Poincare::Context * globalContext);
  double xValueForYValue(int series, double y, Poincare::Context * globalContext);
  double residualAtIndexForSeries(int series, int index, Poincare::Context * globalContext);

  // To speed up computation during drawings, float is returned.
  float maxValueOfColumn(int series, int i) const;
  float minValueOfColumn(int series, int i) const;
private:
  double computeDeterminationCoefficient(int series, Poincare::Context * globalContext);
  void resetMemoization();
  Model * regressionModel(int index);
  uint32_t m_seriesChecksum[k_numberOfSeries];
  Model::Type * m_regressionTypes; // This is a table of size k_numberOfSeries.
  NoneModel m_noneModel;
  LinearModel m_linearModel;
  ProportionalModel m_proportionalModel;
  QuadraticModel m_quadraticModel;
  CubicModel m_cubicModel;
  QuarticModel m_quarticModel;
  LogarithmicModel m_logarithmicModel;
  ExponentialModel m_exponentialAebxModel;
  ExponentialModel m_exponentialAbxModel;
  PowerModel m_powerModel;
  TrigonometricModel m_trigonometricModel;
  LogisticModel m_logisticModel;
  MedianModel m_medianModel;
  double m_regressionCoefficients[k_numberOfSeries][Model::k_maxNumberOfCoefficients];
  double m_determinationCoefficient[k_numberOfSeries];
  bool m_regressionChanged[k_numberOfSeries];
};

typedef double (Store::*ArgCalculPointer)(int, int, bool) const;
typedef void (Store::*RangeMethodPointer)();

}

#endif
