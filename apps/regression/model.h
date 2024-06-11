#ifndef REGRESSION_MODEL_H
#define REGRESSION_MODEL_H

#include <escher/i18n.h>
#include <poincare/src/regression/regression.h>

namespace Regression {

class Store;

class StoreToSeries : public Poincare::Regression::Series {
 public:
  StoreToSeries(Store* store, int series) : m_store(store), m_series(series) {}
  double getX(int i) const override;
  double getY(int i) const override;
  int numberOfPairs() const override;

 private:
  Store* m_store;
  int m_series;
};

class Model {
 public:
  using Type = Poincare::Regression::Regression::Type;

  Model(Type type) : m_type(type) {}

  const char* formula() const {
    if (useLinearMxpbForm()) {
      return "y=m·x+b";
    }
    return regression()->formula();
  }
  I18n::Message name() const;
  int numberOfCoefficients() const {
    return regression()->numberOfCoefficients();
  }

  Poincare::Layout templateLayout() const {
    if (useLinearMxpbForm()) {
      return Poincare::Layout::String("m·x+b");
    }
    return regression()->templateLayout();
  };
  Poincare::Layout equationLayout(
      double* modelCoefficients, const char* ySymbol, int significantDigits,
      Poincare::Preferences::PrintFloatMode displayMode) const {
    return regression()->equationLayout(modelCoefficients, ySymbol,
                                        significantDigits, displayMode);
  };
  Poincare::UserExpression expression(double* modelCoefficients) const {
    return regression()->expression(modelCoefficients);
  };

  /* Evaluate cannot use the expression and approximate it since it would be
   * too time consuming. */
  double evaluate(double* modelCoefficients, double x) const {
    return regression()->evaluate(modelCoefficients, x);
  };
  double levelSet(double* modelCoefficients, double xMin, double xMax, double y,
                  Poincare::Context* context) {
    return regression()->levelSet(modelCoefficients, xMin, xMax, y, context);
  };
  void fit(Store* store, int series, double* modelCoefficients,
           Poincare::Context* context) {
    StoreToSeries bridge(store, series);
    return regression()->fit(&bridge, modelCoefficients, context);
  }

  constexpr static auto k_numberOfModels =
      Poincare::Regression::Regression::k_numberOfModels;
  constexpr static auto k_xSymbol = Poincare::Regression::Regression::k_xSymbol;
  constexpr static auto k_maxNumberOfCoefficients =
      Poincare::Regression::Regression::k_maxNumberOfCoefficients;

 private:
  bool useLinearMxpbForm() const;
  const Poincare::Regression::Regression* regression() const {
    return Poincare::Regression::Regression::Get(m_type);
  }
  Type m_type;
};

}  // namespace Regression

#endif
