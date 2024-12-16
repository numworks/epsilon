#include <emscripten/bind.h>
#include <poincare/src/regression/regression.h>
#include <poincare/src/regression/series.h>

#include <string>

#include "typed_expression.h"
#include "utils.h"

using namespace emscripten;

namespace Poincare::JSBridge {

class SeriesFromJsArray : public Regression::Series {
 public:
  SeriesFromJsArray(const FloatArray& xArray, const FloatArray& yArray)
      : m_xArray(xArray), m_yArray(yArray) {}

  int numberOfPairs() const override {
    return Utils::ArraysHaveSameLength(m_xArray, m_yArray)
               ? m_xArray["length"].as<int>()
               : 0;
  }
  double getX(int i) const override { return m_xArray[i].as<double>(); }
  double getY(int i) const override { return m_yArray[i].as<double>(); }

  const FloatArray& getXArray() const { return m_xArray; }
  const FloatArray& getYArray() const { return m_yArray; }

  double columProductSumWrapper() const {
    return columnProductSum(StatisticsCalculationOptions());
  }
  double covarianceWrapper() const {
    return covariance(StatisticsCalculationOptions());
  }
  double slopeWrapper() const { return slope(StatisticsCalculationOptions()); }
  double yInterceptWrapper() const {
    return yIntercept(StatisticsCalculationOptions());
  }

 private:
  const FloatArray m_xArray;
  const FloatArray m_yArray;
};

/* Methods of Regression using double* modelCoefficients are wrapped with a
 * ModelCoefficients version, which is an std::array that is mapped to JS Array
 * by value_array. */

using ModelCoefficients =
    std::array<double, Regression::Regression::k_maxNumberOfCoefficients>;

ModelCoefficients fit(const Regression::Regression* reg,
                      const SeriesFromJsArray* series) {
  ModelCoefficients result;
  result.fill(NAN);
  reg->fit(series, result.data(), nullptr);
  return result;
}

std::string latexTemplateFormula(const Regression::Regression* reg) {
  constexpr int k_bufferSize = 1024;
  char buffer[k_bufferSize];
  Layout layout = reg->templateLayout();
  layout.toLatex(buffer, k_bufferSize);
  return std::string(buffer, strlen(buffer));
}

TypedUserExpression expression(const Regression::Regression* reg,
                               const ModelCoefficients modelCoefficients) {
  return TypedUserExpression::Cast(reg->expression(modelCoefficients.data()));
}

double evaluate(const Regression::Regression* reg,
                const ModelCoefficients modelCoefficients, double x) {
  return reg->evaluate(modelCoefficients.data(), x);
}

double correlationCoefficient(const Regression::Regression* reg,
                              const SeriesFromJsArray* series) {
  return reg->correlationCoefficient(series);
}

double determinationCoefficient(const Regression::Regression* reg,
                                const SeriesFromJsArray* series,
                                const ModelCoefficients modelCoefficients) {
  return reg->determinationCoefficient(series, modelCoefficients.data());
}

double residualAtIndex(const Regression::Regression* reg,
                       const SeriesFromJsArray* series,
                       const ModelCoefficients modelCoefficients, int index) {
  return reg->residualAtIndex(series, modelCoefficients.data(), index);
}

double residualStandardDeviation(const Regression::Regression* reg,
                                 const SeriesFromJsArray* series,
                                 const ModelCoefficients modelCoefficients) {
  return reg->residualStandardDeviation(series, modelCoefficients.data());
}

EMSCRIPTEN_BINDINGS(regression) {
  class_<SeriesFromJsArray>("PCR_RegressionSeries")
      .constructor<const FloatArray&, const FloatArray&>()
      .function("numberOfPairs", &SeriesFromJsArray::numberOfPairs)
      .function("getXArray", &SeriesFromJsArray::getXArray)
      .function("getYArray", &SeriesFromJsArray::getYArray)
      .function("columnProductSum", &SeriesFromJsArray::columProductSumWrapper)
      .function("covariance", &SeriesFromJsArray::covarianceWrapper)
      .function("slope", &SeriesFromJsArray::slopeWrapper)
      .function("yIntercept", &SeriesFromJsArray::yInterceptWrapper);

  enum_<Regression::Regression::Type>("RegressionType")
      .value("None", Regression::Regression::Type::None)
      .value("LinearAxpb", Regression::Regression::Type::LinearAxpb)
      .value("Proportional", Regression::Regression::Type::Proportional)
      .value("Quadratic", Regression::Regression::Type::Quadratic)
      .value("Cubic", Regression::Regression::Type::Cubic)
      .value("Quartic", Regression::Regression::Type::Quartic)
      .value("Logarithmic", Regression::Regression::Type::Logarithmic)
      .value("ExponentialAebx", Regression::Regression::Type::ExponentialAebx)
      .value("ExponentialAbx", Regression::Regression::Type::ExponentialAbx)
      .value("Power", Regression::Regression::Type::Power)
      .value("Trigonometric", Regression::Regression::Type::Trigonometric)
      .value("Logistic", Regression::Regression::Type::Logistic)
      .value("Median", Regression::Regression::Type::Median)
      .value("LinearApbx", Regression::Regression::Type::LinearApbx);

  class_<Regression::Regression>("PCR_Regression")
      /* The regression object return by Get is a static const object,
       * policy::reference tells JS it does not have the ownership on it. */
      .constructor(&Regression::Regression::Get,
                   return_value_policy::reference())
      .function("type", &Regression::Regression::type)
      .function("hasR", &Regression::Regression::hasR)
      .function("hasRSquared", &Regression::Regression::hasRSquared)
      .function("hasR2", &Regression::Regression::hasR2)
      .function("numberOfCoefficients",
                &Regression::Regression::numberOfCoefficients)
      .function("latexTemplateFormula", &latexTemplateFormula,
                allow_raw_pointers())
      .function("expression", &expression, allow_raw_pointers())
      .function("fit", &fit, allow_raw_pointers())
      .function("evaluate", &evaluate, allow_raw_pointers())
      .function("correlationCoefficient", &correlationCoefficient,
                allow_raw_pointers())
      .function("determinationCoefficient", &determinationCoefficient,
                allow_raw_pointers())
      .function("residualAtIndex", &residualAtIndex, allow_raw_pointers())
      .function("residualStandardDeviation", &residualStandardDeviation,
                allow_raw_pointers());

  static_assert(std::size(ModelCoefficients()) == 5);
  value_array<ModelCoefficients>("ModelCoefficients")
      .element(emscripten::index<0>())
      .element(emscripten::index<1>())
      .element(emscripten::index<2>())
      .element(emscripten::index<3>())
      .element(emscripten::index<4>());
}

}  // namespace Poincare::JSBridge
