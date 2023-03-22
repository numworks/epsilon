#ifndef REGRESSION_STORE_H
#define REGRESSION_STORE_H

#include <apps/shared/double_pair_store_preferences.h>
#include <apps/shared/interactive_curve_view_range.h>
#include <apps/shared/linear_regression_store.h>
#include <escher/responder.h>
#include <float.h>

#include "model/cubic_model.h"
#include "model/exponential_model.h"
#include "model/linear_model.h"
#include "model/logarithmic_model.h"
#include "model/logistic_model.h"
#include "model/median_model.h"
#include "model/model.h"
#include "model/none_model.h"
#include "model/power_model.h"
#include "model/proportional_model.h"
#include "model/quadratic_model.h"
#include "model/quartic_model.h"
#include "model/trigonometric_model.h"

namespace Regression {

class Store : public Shared::LinearRegressionStore {
 public:
  constexpr static const char* const* k_columnNames =
      DoublePairStore::k_regressionColumNames;
  static const char* SeriesTitle(int series);
  constexpr static const char* k_functionName = "R";

  Store(Shared::GlobalContext* context,
        Shared::DoublePairStorePreferences* preferences,
        Model::Type* regressionTypes);

  void reset();

  // Regression
  void setSeriesRegressionType(int series, Model::Type type);
  Model::Type seriesRegressionType(int series) {
    return m_regressionTypes[series];
  }
  Model* modelForSeries(int series) {
    assert(series >= 0 && series < k_numberOfSeries);
    assert((int)m_regressionTypes[series] >= 0 &&
           (int)m_regressionTypes[series] < Model::k_numberOfModels);
    return regressionModel(m_regressionTypes[series]);
  }

  // Dots
  int closestVerticalDot(OMG::VerticalDirection direction, double x, double y,
                         int currentSeries, int currentDot, int* nextSeries,
                         Poincare::Context* globalContext);
  int nextDot(int series, OMG::HorizontalDirection direction, int dot,
              bool displayMean);
  Model* regressionModel(Model::Type type) {
    return regressionModel(static_cast<int>(type));
  }

  // Series
  void updateSeriesValidity(int series,
                            bool updateDisplayAdditionalColumn = true) override;

  // Calculation
  void updateCoefficients(int series, Poincare::Context* globalContext);
  double* coefficientsForSeries(int series, Poincare::Context* globalContext);
  bool coefficientsAreDefined(int series, Poincare::Context* globalContext);
  // R2
  double determinationCoefficientForSeries(int series,
                                           Poincare::Context* globalContext);
  double yValueForXValue(int series, double x,
                         Poincare::Context* globalContext);
  double xValueForYValue(int series, double y,
                         Poincare::Context* globalContext);
  double residualAtIndexForSeries(int series, int index,
                                  Poincare::Context* globalContext);
  double residualStandardDeviation(int series,
                                   Poincare::Context* globalContext);
  bool seriesNumberOfAbscissaeGreaterOrEqualTo(int series, int i) const;

  // To speed up computation during drawings, float is returned.
  float maxValueOfColumn(int series, int i) const;
  float minValueOfColumn(int series, int i) const;

  // Double Pair Store
  bool updateSeries(int series, bool delayUpdate = false,
                    bool updateDisplayAdditionalColumn = true) override;

 private:
  double computeDeterminationCoefficient(int series,
                                         Poincare::Context* globalContext);
  void resetMemoization();
  Model* regressionModel(int index);

  constexpr static int k_functionNameSize = 3;
  static int BuildFunctionName(int series, char* buffer, int bufferSize);
  Ion::Storage::Record functionRecord(int series) const;
  void storeRegressionFunction(int series,
                               Poincare::Expression expression) const;
  void deleteRegressionFunction(int series) const;

  // This is a table of size k_numberOfSeries.
  Model::Type* m_regressionTypes;

  NoneModel m_noneModel;
  LinearModel m_linearAxpbModel;
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
  LinearModel m_linearApbxModel;
  double m_regressionCoefficients[k_numberOfSeries]
                                 [Model::k_maxNumberOfCoefficients];
  double m_determinationCoefficient[k_numberOfSeries];
  bool m_recomputeCoefficients[k_numberOfSeries];
};

typedef void (Store::*RangeMethodPointer)();

}  // namespace Regression

#endif
