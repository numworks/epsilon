#include <omg/unreachable.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>
#include <poincare/regression/regression.h>

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
#include "trigonometric_regression.h"

namespace Poincare::Regression {

/* WARNING: The "constexpr static" object sometimes raised "memory out of
 * bounds" error with emscripten compiler for an unknown reason.
 * Initilazing them at runtime (not constexpr) fixed the problem.
 * This might need further investigation ? Does the emscripten compiler respects
 * all the cpp standard guarantees ? Note that changing -Oz to -O0 doesn't fix
 * the problem. Weirdly, using ASSERTIONS=1 fixes it, no idea why. */
#ifdef TARGET_POINCARE_JS
#define STATIC_VARIABLE static
#else
#define STATIC_VARIABLE constexpr static
#endif

const Regression* Regression::Get(Type type, Preferences::AngleUnit angleUnit) {
  STATIC_VARIABLE NoneRegression none;
  STATIC_VARIABLE LinearRegression linearAxpb(false);
  STATIC_VARIABLE LinearRegression linearApbx(true);
  STATIC_VARIABLE ProportionalRegression proportional;
  STATIC_VARIABLE QuadraticRegression quadratic;
  STATIC_VARIABLE CubicRegression cubic;
  STATIC_VARIABLE QuarticRegression quartic;
  STATIC_VARIABLE LogarithmicRegression logarithmic;
  STATIC_VARIABLE ExponentialRegression exponentialAebx(false);
  STATIC_VARIABLE ExponentialRegression exponentialAbx(true);
  STATIC_VARIABLE PowerRegression power;
  STATIC_VARIABLE LogisticRegression logistic;
  STATIC_VARIABLE MedianRegression median;
  /* NOTE: Having a static var for each angle unit seems weird, but it
   * was the easiest way to adapt to the current implementation.
   * Maybe the way Regressions are handled should be rethought ? */
  STATIC_VARIABLE TrigonometricRegression trigonometricRad(
      Preferences::AngleUnit::Radian);
  STATIC_VARIABLE TrigonometricRegression trigonometricDeg(
      Preferences::AngleUnit::Degree);
  STATIC_VARIABLE TrigonometricRegression trigonometricGrad(
      Preferences::AngleUnit::Gradian);

  switch (type) {
    case Type::None:
      return &none;
    case Type::LinearAxpb:
      return &linearAxpb;
    case Type::LinearApbx:
      return &linearApbx;
    case Type::Proportional:
      return &proportional;
    case Type::Quadratic:
      return &quadratic;
    case Type::Cubic:
      return &cubic;
    case Type::Quartic:
      return &quartic;
    case Type::Logarithmic:
      return &logarithmic;
    case Type::ExponentialAebx:
      return &exponentialAebx;
    case Type::ExponentialAbx:
      return &exponentialAbx;
    case Type::Power:
      return &power;
    case Type::Logistic:
      return &logistic;
    case Type::Median:
      return &median;
    case Type::Trigonometric: {
      switch (angleUnit) {
        case Preferences::AngleUnit::Radian:
          return &trigonometricRad;
        case Preferences::AngleUnit::Degree:
          return &trigonometricDeg;
        case Preferences::AngleUnit::Gradian:
          return &trigonometricGrad;
        default:
          OMG::unreachable();
      }
    }
  }
  OMG::unreachable();
}

int Regression::NumberOfCoefficients(Type type) {
  switch (type) {
    case Type::None:
      return 0;
    case Type::Proportional:
      return 1;
    case Type::LinearAxpb:
    case Type::LinearApbx:
    case Type::Logarithmic:
    case Type::ExponentialAebx:
    case Type::ExponentialAbx:
    case Type::Power:
    case Type::Median:
      return 2;
    case Type::Quadratic:
    case Type::Logistic:
      return 3;
    case Type::Cubic:
    case Type::Trigonometric:
      return 4;
    case Type::Quartic:
      return 5;
  }
  OMG::unreachable();
}

const char* Regression::Formula(Type type) {
  switch (type) {
    case Type::None:
      assert(false);
      return "";
    case Type::LinearAxpb:
    case Type::Median:
      return "y=a·x+b";
    case Type::LinearApbx:
      return "y=a+b·x";
    case Type::Proportional:
      return "y=a·x";
    case Type::Quadratic:
      return "y=a·x^2+b·x+c";
    case Type::Cubic:
      return "y=a·x^3+b·x^2+c·x+d";
    case Type::Quartic:
      return "y=a·x^4+b·x^3+c·x^2+d·x+e";
    case Type::Logarithmic:
      return "y=a+b·ln(x)";
    case Type::ExponentialAebx:
      return "y=a·exp(b·x)";
    case Type::ExponentialAbx:
      return "y=a·b^x";
    case Type::Power:
      return "y=a·x^b";
    case Type::Trigonometric:
      return "y=a·sin(b·x+c)+d";
    case Type::Logistic:
      return "y=c/(1+a·exp(-b·x))";
  }
  OMG::unreachable();
}

const Poincare::Layout Regression::TemplateLayout(Type type) {
  switch (type) {
    case Type::None:
      assert(false);
      return Poincare::Layout();
    case Type::Quadratic:
      return "a·x"_l ^ KSuperscriptL("2"_l) ^ "+b·x+c"_l;
    case Type::Cubic:
      return "a·x"_l ^ KSuperscriptL("3"_l) ^ "+b·x"_l ^ KSuperscriptL("2"_l) ^
             "+c·x+d"_l;
    case Type::Quartic:
      return "a·x"_l ^ KSuperscriptL("4"_l) ^ "+b·x"_l ^ KSuperscriptL("3"_l) ^
             "+c·x"_l ^ KSuperscriptL("2"_l) ^ "+d·x+e"_l;
    case Type::ExponentialAebx:
      return "a·e"_l ^ KSuperscriptL("b·x"_l);
    case Type::ExponentialAbx:
      return "a·b"_l ^ KSuperscriptL("x"_l);
    case Type::Power:
      return "a·x"_l ^ KSuperscriptL("b"_l);
    case Type::Logistic:
      return KRackL(KFracL("c"_l, "1+a·e"_l ^ KSuperscriptL("-b·x"_l)));
    default:
      return Layout::String(Formula(type) + sizeof("y=") - 1);
  }
  OMG::unreachable();
}

}  // namespace Poincare::Regression
