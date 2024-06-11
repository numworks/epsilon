#include <emscripten/bind.h>
#include <poincare/src/regression/linear_regression.h>
#include <poincare/src/regression/regression.h>

using namespace emscripten;

namespace Poincare::JSBridge {

/* Wrapper to allow Series to be subclassed by JS */
struct SeriesWrapper : public wrapper<Regression::Series> {
  EMSCRIPTEN_WRAPPER(SeriesWrapper);
  double getX(int i) const { return call<double>("getX", i); }
  double getY(int i) const { return call<double>("getY", i); }
  int numberOfPairs() const { return call<int>("numberOfPairs"); }
};

/* Methods of Regression using double* modelCoefficients are wrapped with a
 * ModelCoefficients version, which is an std::array that is mapped to JS Array
 * by value_array. */

using ModelCoefficients =
    std::array<double, Regression::Regression::k_maxNumberOfCoefficients>;

ModelCoefficients fit(const Regression::Regression* reg,
                      const Regression::Series* series) {
  ModelCoefficients result;
  result.fill(NAN);
  reg->fit(series, result.data(), nullptr);
  return result;
}

double evaluate(const Regression::Regression* reg,
                const ModelCoefficients ModelCoefficients, double x) {
  return reg->evaluate(ModelCoefficients.data(), x);
}

EMSCRIPTEN_BINDINGS(regression) {
  class_<Regression::Series>("Series").allow_subclass<SeriesWrapper>(
      "SeriesWrapper");

  enum_<Regression::Regression::Type>("RegressionType")
      .value("Linear", Regression::Regression::Type::LinearAxpb)
      .value("Quadratic", Regression::Regression::Type::Quadratic)
      .value("Cubic", Regression::Regression::Type::Cubic);

  class_<Regression::Regression>("Regression")
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

/* Usage example :
  var ArraySeries = Poincare.Series.extend("Series", {
    __construct: function(x, y) {
      if (x.length != y.length) {
        console.log("bad array")
          }
      this.x = x
        this.y = y
        this.__parent.__construct.call(this);
    },
      getX: function(i) {
      return this.x[i];
    },
      getY: function(i) {
      return this.y[i];
    },
      numberOfPairs: function() {
      return this.x.length;
    },
  });

  var series = new ArraySeries([1.0, 8.0, 14.0, 79.0],
                               [-3.581, 20.296, 40.676, 261.623]);
  var regression = new Poincare.Regression(Poincare.RegressionType.Linear);
  var coefficients = regression.fit(series);
  var prediction = regression.evaluate(coefficients, 10);
*/

}  // namespace Poincare::JSBridge
