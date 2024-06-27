#include <omg/unreachable.h>

#include "cubic_regression.h"
#include "exponential_regression.h"
#include "linear_regression.h"
#include "logarithmic_regression.h"
#include "logistic_regression.h"
#include "median_regression.h"
#include "none_regression.h"
#include "power_regression.h"
#include "proportional_regression.h"
#include "quadratic_regression.h"
#include "quartic_regression.h"
#include "regression.h"
#include "trigonometric_regression.h"

namespace Poincare::Regression {
const Regression* Regression::Get(Type type) {
  switch (type) {
    case Type::None:
      constexpr static NoneRegression none;
      return &none;
    case Type::LinearAxpb:
      constexpr static LinearRegression linearAbxpb(false);
      return &linearAbxpb;
    case Type::LinearApbx:
      constexpr static LinearRegression linearApbx(true);
      return &linearApbx;
    case Type::Proportional:
      constexpr static ProportionalRegression proportional;
      return &proportional;
    case Type::Quadratic:
      constexpr static QuadraticRegression quadratic;
      return &quadratic;
    case Type::Cubic:
      constexpr static CubicRegression cubic;
      return &cubic;
    case Type::Quartic:
      constexpr static QuarticRegression quartic;
      return &quartic;
    case Type::Logarithmic:
      constexpr static LogarithmicRegression logarithmic;
      return &logarithmic;
    case Type::ExponentialAebx:
      constexpr static ExponentialRegression exponentialAebx(false);
      return &exponentialAebx;
    case Type::ExponentialAbx:
      constexpr static ExponentialRegression exponentialAbx(true);
      return &exponentialAbx;
    case Type::Power:
      constexpr static PowerRegression power;
      return &power;
    case Type::Trigonometric:
      constexpr static TrigonometricRegression trigonometric;
      return &trigonometric;
    case Type::Logistic:
      constexpr static LogisticRegression logistic;
      return &logistic;
    case Type::Median:
      constexpr static MedianRegression median;
      return &median;
  }
  OMG::unreachable();
}

}  // namespace Poincare::Regression
