#pragma once

#include <apps/global_preferences.h>
#include <apps/shared/double_pair_store_preferences.h>
#include <apps/shared/interactive_curve_view_range.h>
#include <apps/shared/linear_regression_store.h>
#include <escher/responder.h>
#include <float.h>
#include <omg/string.h>

#include "model.h"

namespace Regression {

class Store : public Shared::LinearRegressionStore {
 public:
  constexpr static const char* const* k_columnNames =
      DoublePairStore::k_regressionColumNames;
  static const char* SeriesTitle(int series);

  constexpr static char k_regressionFunctionBaseName[] = "R";
  static constexpr const char* const k_RegressionFunctionBaseNames[] = {
      Store::k_regressionFunctionBaseName};

  constexpr static char k_residualListBaseName[] = "RES";
  static constexpr const char* const k_ResidualListBaseNames[] = {
      Store::k_residualListBaseName};

  constexpr static std::array<const char*, 2> k_storedNames = {
      k_regressionFunctionBaseName, k_residualListBaseName};
  constexpr static int k_nameBufferLength =
      std::string_view(*std::max_element(k_storedNames.begin(),
                                         k_storedNames.end(),
                                         OMG::StringLengthCompare))
          .length()
      // + 1 for the series index
      + 1
      // + 1 for the null-terminating character
      + 1;

  Store(Shared::DoublePairStorePreferences* preferences,
        Model::Type* regressionTypes);

  void reset();

  // Regression
  void setSeriesRegressionType(int series, Model::Type type);
  Model::Type seriesRegressionType(int series) const {
    return m_regressionTypes[series];
  }
  Model* modelForSeries(int series) const {
    assert(series >= 0 && series < k_numberOfSeries);
    assert((int)m_regressionTypes[series] >= 0 &&
           (int)m_regressionTypes[series] < Model::k_numberOfModels);
    return regressionModel(m_regressionTypes[series]);
  }

  // Dots
  int closestVerticalDot(OMG::VerticalDirection direction, double x, double y,
                         int currentSeries, int currentDot, int* nextSeries);
  int nextDot(int series, OMG::HorizontalDirection direction, int dot,
              bool displayMean);
  Model* regressionModel(Model::Type type) const;

  // Series
  void updateSeriesValidity(int series) override;

  // Calculation
  void updateCoefficients(int series);
  double* coefficientsForSeries(int series);
  bool coefficientsAreDefined(int series, bool finite = false);
  double correlationCoefficient(int series) const;
  // R2
  double determinationCoefficientForSeries(int series);
  double yValueForXValue(int series, double x);
  double xValueForYValue(int series, double y);
  double residualAtIndexForSeries(int series, int index);
  double residualStandardDeviation(int series);

  // To speed up computation during drawings, float is returned.
  float maxValueOfColumn(int series, int i) const;
  float minValueOfColumn(int series, int i) const;

  // Double Pair Store
  bool updateSeries(int series, bool delayUpdate = false) override;

  // Type-specific properties
  typedef bool (*TypeProperty)(Model::Type type);
  static bool HasCoefficients(Model::Type type) {
    return Poincare::Regression::HasCoefficients(type);
  }
  static bool DisplayR(Model::Type type) {
    return Poincare::Regression::HasR(type);
  }
  static bool DisplayRSquared(Model::Type type) {
    return Poincare::Regression::HasRSquared(type);
  }
  static bool DisplayR2(Model::Type type) {
    return Poincare::Regression::HasR2(type);
  }
  static bool DisplayResidualStandardDeviation(Model::Type type) {
    return HasCoefficients(type) &&
           SharedGlobalPrefs().regressionAppVariant() ==
               CountryPreferences::RegressionApp::Variant1;
  }
  static bool HasCoefficientM(Model::Type type) {
    return SharedGlobalPrefs().regressionAppVariant() ==
               CountryPreferences::RegressionApp::Variant1 &&
           (type == Model::Type::LinearAxpb || type == Model::Type::Median);
  }
  static bool HasCoefficientA(Model::Type type) {
    // Any regression type not having M coefficient have A coefficient
    return HasCoefficients(type) && !HasCoefficientM(type);
  }
  bool AnyActiveSeriesSatisfies(TypeProperty property) const;
  bool seriesSatisfies(int series, TypeProperty property) const {
    return property(seriesRegressionType(series));
  }

 private:
  double computeDeterminationCoefficient(int series);
  double computeResidualStandardDeviation(int series);
  void resetMemoization();

  constexpr static const char* ExtensionForBaseName(std::string_view baseName) {
    return baseName.starts_with("RES")
               ? Ion::Storage::regressionListExtension
               : Ion::Storage::regressionFunctionExtension;
  }

  static int BuildStoredName(const char* baseName, int series,
                             std::span<char> buffer);
  Ion::Storage::Record storedRecord(const char* baseName, int series) const;
  void storeElement(const char* baseName, int series,
                    const Poincare::UserExpression& expression) const;
  void deleteStoredElement(const char* baseName, int series) const;
  void deleteStoredElements(int series) const;

  // This is a table of size k_numberOfSeries.
  Model::Type* m_regressionTypes;

  double m_regressionCoefficients[k_numberOfSeries]
                                 [Model::k_maxNumberOfCoefficients];
  double m_determinationCoefficient[k_numberOfSeries];
  double m_residualStandardDeviation[k_numberOfSeries];
  bool m_recomputeCoefficients[k_numberOfSeries];

  // TODO: an std::string_view could be used
  using stringViewType = const char*;
  constexpr static std::array<stringViewType, k_numberOfSeries> k_seriesTitles =
      {"X1/Y1", "X2/Y2", "X3/Y3", "X4/Y4", "X5/Y5", "X6/Y6"};
};

typedef void (Store::*RangeMethodPointer)();

}  // namespace Regression
