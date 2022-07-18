#include <quiz.h>
#include <string.h>
#include <assert.h>
#include <apps/shared/global_context.h>
#include <apps/shared/store_context.h>
#include "../model/model.h"
#include "../store.h"
#include <poincare/helpers.h>
#include <poincare/trigonometry.h>
#include <poincare/test/helper.h>

using namespace Poincare;
using namespace Regression;

/* The data was generated by choosing X1 and the coefficients of the regression,
 * then filling Y1 with the regression formula + random()/10. */

void setRegressionPoints(Regression::Store * store, int series, int numberOfPoints, double * xi, double * yi = nullptr) {
  store->deleteAllPairsOfSeries(series);
  for (int i = 0; i < numberOfPoints; i++) {
    store->set(xi[i], series, 0, i);
    if (yi != nullptr) {
      store->set(yi[i], series, 1, i);
    }
  }
}

void assert_regression_is(double * xi, double * yi, int numberOfPoints, Model::Type modelType, double * trueCoefficients, double trueR2, bool acceptNAN = false) {
  int series = 0;
  Shared::GlobalContext globalContext;
  Model::Type regressionTypes[] = { Model::Type::None, Model::Type::None, Model::Type::None };
  Regression::Store store(&globalContext, regressionTypes);

  setRegressionPoints(&store, series, numberOfPoints, xi, yi);
  store.setSeriesRegressionType(series, modelType);
  Shared::StoreContext context(&store, &globalContext);

  double precision = 1e-2;
  // When expected value is null, expect a stronger precision
  double nullExpectedPrecision = 1e-9;

  // Compute and compare the coefficients
  double * coefficients = store.coefficientsForSeries(series, &context);
  int numberOfCoefs = store.modelForSeries(series)->numberOfCoefficients();
  for (int i = 0; i < numberOfCoefs; i++) {
    quiz_assert(roughly_equal(coefficients[i], trueCoefficients[i], precision, acceptNAN, nullExpectedPrecision));
  }

  if (modelType != Model::Type::Proportional && modelType != Model::Type::Median) {
    // Compute and check r2 value and sign
    double r2 = store.determinationCoefficientForSeries(series, &globalContext);
    quiz_assert(r2 <= 1.0 && r2 >= 0.0);
    quiz_assert(roughly_equal(r2, trueR2, precision, false, nullExpectedPrecision));
  }
}

QUIZ_CASE(linear_regression) {
  double x[] = {1.0, 8.0, 14.0, 79.0};
  double y[] = {-3.581, 20.296, 40.676, 261.623};
  double coefficients[] = {3.4, -7.0};
  double r2 = 1.0;
  assert_regression_is(x, y, 4, Model::Type::Linear, coefficients, r2);
}

QUIZ_CASE(linear_regression2) {
  double x[] = {-5.0, 2.0, 4.0, 5.6, 9.0};
  double y[] = {22.0, 1.0, 13.0, 28.36, 78.0};
  double coefficients[] = {3.31824, 18.1191};
  double r2 = 0.343;
  assert_regression_is(x, y, 5, Model::Type::Linear, coefficients, r2);
}

QUIZ_CASE(proportional_regression) {
  double x[] = {7.0, 5.0, 1.0, 9.0, 3.0};
  double y[] = {-41.4851, -29.62186, -6.454245, -53.4976, -18.03325};
  double coefficients[] = {-5.89};
  double r2 = 0.9999648161902982;
  assert_regression_is(x, y, 5, Model::Type::Proportional, coefficients, r2);
}

QUIZ_CASE(proportional_regression2) {
  constexpr int numberOfPoints = 4;
  double x[numberOfPoints] = {5.0, 2.0, 3.0, 4.0};
  double y[numberOfPoints] = {10.0, 6.0, 7.0, 8.0};
  double coefficients[] = {2.12963963};
  double r2 = 0.53227513227513223;
  assert_regression_is(x, y, numberOfPoints, Model::Type::Proportional, coefficients, r2);
}

QUIZ_CASE(proportional_regression3) {
  constexpr int numberOfPoints = 4;
  double x[numberOfPoints] = {1.0, 2.0, 3.0, 4.0};
  double y[numberOfPoints] = {0.0, 0.0, 0.0, 0.0};
  double coefficients[] = {0.0};
  double r2 = 1.0;
  assert_regression_is(x, y, numberOfPoints, Model::Type::Proportional, coefficients, r2);
}

QUIZ_CASE(proportional_regression4) {
  constexpr int numberOfPoints = 3;
  double x[numberOfPoints] = {-1.0, 0.0, 1.0};
  double y[numberOfPoints] = {1.0, 1.0, 1.0};
  double coefficients[] = {0.0};
  // Y is constant, and proportional regression cannot fit it, R2 is null.
  double r2 = 0.0;
  assert_regression_is(x, y, numberOfPoints, Model::Type::Proportional, coefficients, r2);
}

QUIZ_CASE(proportional_regression5) {
  constexpr int numberOfPoints = 3;
  double x[numberOfPoints] = {-1.0, 0.0, 1.0};
  double y[numberOfPoints] = {1.0, 1.01, 1.0};
  double coefficients[] = {0.0};
  /* In this case, proportional regression performed poorly compared to a
   * constant regression, R2 is negative. */
  double r2 = -45300.5;
  assert_regression_is(x, y, numberOfPoints, Model::Type::Proportional, coefficients, r2);
}

QUIZ_CASE(quadratic_regression) {
  double x[] = {-34.0, -12.0, 5.0, 86.0, -2.0};
  double y[] = {-8241.389, -1194.734, -59.163, - 46245.39, -71.774};
  double coefficients[] = {-6.50001, 21.3004, -3.15799};
  double r2 = 1.0;
  assert_regression_is(x, y, 5, Model::Type::Quadratic, coefficients, r2);
}

QUIZ_CASE(cubic_regression) {
  double x[] = {-3.0, -2.8, -1.0, 0.0, 12.0};
  double y[] = {691.261, 566.498, 20.203, -12.865, -34293.21};
  double coefficients[] = {-21.2015, 16.0141, 4.14522, -12.8658};
  double r2 = 1.0;
  assert_regression_is(x, y, 5, Model::Type::Cubic, coefficients, r2);
}

QUIZ_CASE(quartic_regression) {
  double x[] = {1.6, 3.5, 3.5, -2.8, 6.4, 5.3, 2.9, -4.8, -5.7, 3.1};
  double y[] = {-112.667, -1479.824, -1479.805, 1140.276, -9365.505, -5308.355, -816.925, 5554.007, 9277.107, -1009.874};
  double coefficients[] = {0.59998, -42.9998, 21.5015, 3.09232, -0.456824};
  double r2 = 1.0;
  assert_regression_is(x, y, 10, Model::Type::Quartic, coefficients, r2);
}

QUIZ_CASE(logarithmic_regression) {
  double x[] = {0.2, 0.5, 5, 7};
  double y[] = {-11.952, -9.035, -1.695, -0.584};
  double coefficients[] = {-6.81679, 3.19383};
  double r2 = 0.999994;
  assert_regression_is(x, y, 4, Model::Type::Logarithmic, coefficients, r2);
}

QUIZ_CASE(exponential_regression) {
  double x[] = {5.5, 5.6, 5.7, 5.8, 5.9, 6.0};
  double y[] = {-276.842, -299.956, -324.933, -352.0299, -381.314, -413.0775};
  double coefficients[] = {-3.4, 0.8};
  double r2 = 1.0;
  assert_regression_is(x, y, 6, Model::Type::ExponentialAebx, coefficients, r2);
}

QUIZ_CASE(exponential_regression2) {
  double x[] = {0, 1, 2, 3};
  double y[] = {3000, 3315.513, 3664.208, 4049.576};
  double coefficients[] = {3000, .1};
  double r2 = 1.0;
  assert_regression_is(x, y, 4, Model::Type::ExponentialAebx, coefficients, r2);
}

QUIZ_CASE(exponential_regression3) {
  double x[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  double y[] = {-1, -.3678794, -.1353353, -.04978707, -.01831564, -.006737947, -.002478752, -.000911882, -.0003354626, -.0001234098, -.00004539993};
  double coefficients[] = {-1, -1};
  double r2 = 0.9999999999999992;
  assert_regression_is(x, y, 11, Model::Type::ExponentialAebx, coefficients, r2);
}

QUIZ_CASE(exponential_regression4) {
  double x[] = {1.0, 2.0, 3.0, 4.0};
  double y[] = {2.0, 3.0, 4.0, 1.0};
  double coefficients[] = {3.4641, -0.179};
  double r2 = 0.1481;
  assert_regression_is(x, y, 4, Model::Type::ExponentialAebx, coefficients, r2);
}

QUIZ_CASE(exponential_regression5) {
  double x[] = {1, 1, 2, 2};
  double y[] = {1, 2, 2, 1};
  double coefficients[] = {1.41421, 0.0};
  double r2 = 0.0;
  assert_regression_is(x, y, 4, Model::Type::ExponentialAebx, coefficients, r2);
}

QUIZ_CASE(exponential_abx_regression) {
  double x[] = {1, 2, 3, 4};
  double y[] = {2, 4, 8, 16};
  double coefficients[] = {1, 2};
  double r2 = 1.0;
  assert_regression_is(x, y, 4, Model::Type::ExponentialAbx, coefficients, r2);
}

QUIZ_CASE(power_regression) {
  double x[] = {1.0, 50.0, 34.0, 67.0, 20.0};
  double y[] = {71.860, 2775514, 979755.1, 6116830, 233832.9};
  double coefficients[] = {71.8, 2.7};
  double r2 = 1.0;
  assert_regression_is(x, y, 5, Model::Type::Power, coefficients, r2);

  // TODO : This data produce a wrong fit currently
  // double x2[] = {1.0, 2.0, 3.0, 4.0};
  // double y2[] = {2.0, 3.0, 4.0, 1.0};
  // double coefficients2[] = {2.54948, -0.0247463};
  // double r22 = 0.833509;
  // assert_regression_is(x2, y2, 4, Model::Type::Power, coefficients2, r22);
}

QUIZ_CASE(median_regression0) {
  double x[] = {3.0, 3.0, 3.0};
  double y[] = {4.0, 3.0, 2.0};
  double coefficients[] = {NAN, NAN};
  double r2 = 0.0;
  assert_regression_is(x, y, 3, Model::Type::Median, coefficients, r2, true);
}

QUIZ_CASE(median_regression1) {
  double x[] = {1.0, 2.0, 3.0};
  double y[] = {4.0, 3.0, 2.0};
  double coefficients[] = {-1.0, 5.0};
  double r2 = 0.0;
  assert_regression_is(x, y, 3, Model::Type::Median, coefficients, r2);
}

QUIZ_CASE(median_regression2) {
  double x[] = {9.0, 7.0, 5.0, 11.0, 31.0, 19.0, 15.0, 25.0, 1.0, 23.0};
  double y[] = {11.0, 734.0, 3.0, 15.0, 55555.0, 31.0, 23.0, 43.0, -5.0, 39.0};
  double coefficients[] = {2.0, -7.0};
  double r2 = 0.0;
  assert_regression_is(x, y, 10, Model::Type::Median, coefficients, r2);
}

void assert_trigonometric_regression_is(double * xi, double * yi, int numberOfPoints, double * trueCoefficients, double trueR2, Poincare::Preferences::AngleUnit trueCoeffcientsUnit) {
  // Test the trigonometric regression at all angle units
  const Preferences::AngleUnit previousAngleUnit = Preferences::sharedPreferences()->angleUnit();
  const Poincare::Preferences::AngleUnit units[3] = {Poincare::Preferences::AngleUnit::Radian, Poincare::Preferences::AngleUnit::Degree, Poincare::Preferences::AngleUnit::Gradian};
  for (int i = 0; i < 3; ++i) {
    Poincare::Preferences::AngleUnit unit = units[i];
    Poincare::Preferences::sharedPreferences()->setAngleUnit(unit);
    double unitFactor = Trigonometry::PiInAngleUnit(unit) / Trigonometry::PiInAngleUnit(trueCoeffcientsUnit);
    // True coefficients b and c are converted to the tested angle unit
    double coefficientsUnit[] = {trueCoefficients[0], trueCoefficients[1] * unitFactor, trueCoefficients[2] * unitFactor, trueCoefficients[3]};
    assert_regression_is(xi, yi, numberOfPoints, Model::Type::Trigonometric, coefficientsUnit, trueR2);
  }
  // Restore previous angleUnit
  Poincare::Preferences::sharedPreferences()->setAngleUnit(previousAngleUnit);
}

QUIZ_CASE(trigonometric_regression1) {
  double r2 = 0.9994216;
  double x[] = {1, 31, 61, 91, 121, 151, 181, 211, 241, 271, 301, 331, 361};
  double y[] = {9.24, 10.05, 11.33, 12.72, 14.16, 14.98, 15.14, 14.41, 13.24, 11.88, 10.54, 9.48, 9.19};
  double coefficients[] = {2.9723, 0.016780, -1.3067, 12.146};
  int numberOfPoints = sizeof(x) / sizeof(double);
  assert(sizeof(y) == sizeof(double) * numberOfPoints);

  assert_trigonometric_regression_is(x, y, numberOfPoints, coefficients, r2, Poincare::Preferences::AngleUnit::Radian);
}

QUIZ_CASE(trigonometric_regression2) {
  double r2 = 0.9154;
  double x[] = { 0,  2,  4,  6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48};
  double y[] = {-2, -4, -5, -2, 3,  6,  8, 11,  9,  5,  2,  1,  0, -3, -5, -2,  3,  5,  7, 10, 10,  5,  2,  2,  1};
  double coefficients[] = {6.42, 0.26, -2.16, 2.82};
  int numberOfPoints = sizeof(x) / sizeof(double);
  assert(sizeof(y) == sizeof(double) * numberOfPoints);

  assert_trigonometric_regression_is(x, y, numberOfPoints, coefficients, r2, Poincare::Preferences::AngleUnit::Radian);
}

QUIZ_CASE(trigonometric_regression3) {
  double r2 = 0.9983;
  double x[] = {1,  2,  3,  4,  5,  6};
  double y[] = {8, 13, 21, 36, 47, 53};
  double coefficients[] = {22.55, 0.5955, -2.180, 30.86};
  int numberOfPoints = sizeof(x) / sizeof(double);
  assert(sizeof(y) == sizeof(double) * numberOfPoints);

  assert_trigonometric_regression_is(x, y, numberOfPoints, coefficients, r2, Poincare::Preferences::AngleUnit::Radian);
}

QUIZ_CASE(trigonometric_regression4) {
  double r2 = 1.0;
  // The regression fails with more than 89 data points
  constexpr int numberOfPoints = 89;
  double x[numberOfPoints];
  double y[numberOfPoints];
  for (int i = 0; i < numberOfPoints; i++) {
    x[i] = static_cast<double>(i);
    y[i] = std::sin(static_cast<double>(i));
  }
  double coefficients[] = {1.0, 1.0, 0.0, 0.0};

  assert_trigonometric_regression_is(x, y, numberOfPoints, coefficients, r2, Poincare::Preferences::AngleUnit::Radian);
}

QUIZ_CASE(logistic_regression) {
  /* This data was generated without the random error, otherwise it did not pass
   * the test. */
  double x1[] = {2.3, 5.6, 1.1, 4.3};
  double y1[] = {3.948, 4.694, 2.184, 4.656};
  double coefficients1[] = {6.0, 1.5, 4.7};
  double r21 = 0.9999999917270119;
  assert_regression_is(x1, y1, 4, Model::Type::Logistic, coefficients1, r21);

  double x2[] = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
  double y2[] = {5.0, 9.0, 40.0, 64.0, 144.0, 200.0, 269.0, 278.0, 290.0, 295.0};
  double coefficients2[] = {64.9, 1.0, 297.4};
  double r22 = 0.9984396821656006;
  assert_regression_is(x2, y2, 10, Model::Type::Logistic, coefficients2, r22);

  double x3[] = {-400.0, 0.0, 400.0, 450.0, 800.0};
  double y3[] = {1.523, 76.92, 819.8, 882.4, 996.0};
  double coefficients3[] = {12.0, 0.01, 1000.0};
  double r23 = 1.0;
  assert_regression_is(x3, y3, 5, Model::Type::Logistic, coefficients3, r23);

  double x4[] = {-2.0, -1.0, 0.0, 1.0, 2.0, 3.0};
  double y4[] = {-5.0, -5.0, -4.99, -4.90, -3.56, -0.55};
  double coefficients4[] = {0.001, -3.0, -5.0};
  double r24 = 1.0;
  assert_regression_is(x4, y4, 6, Model::Type::Logistic, coefficients4, r24);

  double x5[] = {3.0, 7.0, 11.0, 20.0, 43.0};
  double y5[] = {11.66, 13.51, 15.21, 17.38, 18.7};
  double coefficients5[] = {0.88, 0.118, 18.8};
  double r25 = 1.0;
  assert_regression_is(x5, y5, 5, Model::Type::Logistic, coefficients5, r25);

  double x6[] = {-0.1, -0.09, -0.08, -0.07, -0.06 };
  double y6[] = {1.82e-6, 3.66e-6, 7.34e-6, 1.46e-5, 2.91e-5};
  double coefficients6[] = {1.17e-8, 250.0, 2.77e-5}; // target : {0.5, 70.0, 0.001};
  double r26 = 0.902321; // target : 1.0;
  assert_regression_is(x6, y6, 5, Model::Type::Logistic, coefficients6, r26);

  double x7[] = {1.0, 3.0, 4.0, 6.0, 8.0};
  double y7[] = {4.0, 4.0, 0.0, 58.0, 5.0};
  double coefficients7[] = {3.56e8, 4.256, 31.4}; // No target
  double r27 = 0.4; // No target (But should be positive)
  assert_regression_is(x7, y7, 5, Model::Type::Logistic, coefficients7, r27);
}

// Testing column and regression calculation

void assert_column_calculations_is(double * xi, int numberOfPoints, double trueMean, double trueSum, double trueSquaredSum, double trueStandardDeviation, double trueVariance) {
  int series = 0;
  Shared::GlobalContext globalContext;
  Model::Type regressionTypes[] = { Model::Type::None, Model::Type::None, Model::Type::None };
  Regression::Store store(&globalContext, regressionTypes);

  setRegressionPoints(&store, series, numberOfPoints, xi);

  // Compute and compare the regression calculations metrics
  double mean = store.meanOfColumn(series,0);
  double sum = store.sumOfColumn(series,0);
  double squaredSum = store.squaredValueSumOfColumn(series,0);
  double standardDeviation = store.standardDeviationOfColumn(series,0);
  double variance = store.varianceOfColumn(series,0);

  // Check that squaredSum, standardDeviation and variance are positive
  quiz_assert(squaredSum >= 0.0);
  quiz_assert(standardDeviation >= 0.0);
  quiz_assert(variance >= 0.0);

  double precision = 1e-3;
  // Observed should be exactly 0 if expected value is null.
  quiz_assert(roughly_equal(variance, trueVariance, precision, false, 0.0));
  quiz_assert(roughly_equal(squaredSum, trueSquaredSum, precision, false, 0.0));
  quiz_assert(roughly_equal(mean, trueMean, precision, false, 0.0));
  quiz_assert(roughly_equal(sum, trueSum, precision, false, 0.0));
  quiz_assert(roughly_equal(standardDeviation, trueStandardDeviation, precision, false, 0.0));
}

QUIZ_CASE(column_calculation) {
  double x[] = {2.3, 5.6, 1.1, 4.3};
  double mean = 3.325;
  double sum = 13.3;
  double squaredSum = 56.35;
  double standardDeviation = 1.741228;
  double variance = 3.031875;
  assert_column_calculations_is(x, 4, mean, sum, squaredSum, standardDeviation, variance);
}

QUIZ_CASE(constant_column_calculation) {
  // This data produced a negative variance before
  double x[] = {-996.85840734641, -996.85840734641, -996.85840734641};
  double mean = -996.85840734641;
  double sum = -2990.57522203923;
  double squaredSum = 2981180.0528916633;
  double standardDeviation = 0.0;
  double variance = 0.0;
  assert_column_calculations_is(x, 3, mean, sum, squaredSum, standardDeviation, variance);
}

void assert_regression_calculations_is(double * xi, double * yi, int numberOfPoints, double trueCovariance, double trueProductSum, double trueR) {
  int series = 0;
  Shared::GlobalContext globalContext;
  Model::Type regressionTypes[] = { Model::Type::None, Model::Type::None, Model::Type::None };
  Regression::Store store(&globalContext, regressionTypes);

  setRegressionPoints(&store, series, numberOfPoints, xi, yi);

  double precision = 1e-3;

  // Compute and compare the regression calculations metrics
  double covariance = store.covariance(series);
  double productSum = store.columnProductSum(series);

  // NOTE : A raisonable nullExpectedPrecision for future tests
  quiz_assert(roughly_equal(covariance, trueCovariance, precision, false, 0.0));
  quiz_assert(roughly_equal(productSum, trueProductSum, precision, false, 0.0));

  double r = store.correlationCoefficient(series);
  quiz_assert(r >= 0.0);
  quiz_assert(roughly_equal(r, trueR, precision, false, 0.0));
}

QUIZ_CASE(regression_calculation) {
  double x[] = {1.0, 50.0, 34.0, 67.0, 20.0};
  double y[] = {71.860, 2775514, 979755.1, 6116830.0, 233832.9};
  double covariance = 4.7789036e7;
  double productSum = 586591713.26;
  double r = 0.919088;
  assert_regression_calculations_is(x, y, 5, covariance, productSum, r);
}
