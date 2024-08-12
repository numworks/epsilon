#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <poincare/src/regression/linear_regression.h>
#include <poincare/src/regression/regression.h>
#include <poincare/src/regression/series.h>

using namespace emscripten;

namespace Poincare::JSBridge {

EMSCRIPTEN_DECLARE_VAL_TYPE(FloatArray);

class SeriesFromJsArray : public Regression::Series {
 public:
  SeriesFromJsArray(const FloatArray& xArray, const FloatArray& yArray)
      : m_xArray(xArray), m_yArray(yArray) {}

  int numberOfPairs() const override {
    return ArraysHaveSameLength(m_xArray, m_yArray)
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
  static bool ArraysHaveSameLength(const FloatArray& array,
                                   const FloatArray& otherArray) {
    return array["length"].as<int>() == otherArray["length"].as<int>();
  }

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

double evaluate(const Regression::Regression* reg,
                const ModelCoefficients modelCoefficients, double x) {
  return reg->evaluate(modelCoefficients.data(), x);
}

EMSCRIPTEN_BINDINGS(regression) {
  register_type<FloatArray>("number[]");

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
      .function("numberOfCoefficients",
                &Regression::Regression::numberOfCoefficients)
      .function("fit", &fit, allow_raw_pointers())
      .function("evaluate", &evaluate, allow_raw_pointers());

  static_assert(std::size(ModelCoefficients()) == 5);
  value_array<ModelCoefficients>("ModelCoefficients")
      .element(emscripten::index<0>())
      .element(emscripten::index<1>())
      .element(emscripten::index<2>())
      .element(emscripten::index<3>())
      .element(emscripten::index<4>());
}

}  // namespace Poincare::JSBridge
