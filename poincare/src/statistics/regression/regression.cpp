#include <omg/float.h>
#include <poincare/solver/solver.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/float_helper.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/solver/matrix_array.h>
#include <poincare/src/statistics/dataset_adapter.h>
#include <poincare/statistics/regression.h>

#include <cmath>

using namespace Poincare::Internal;
namespace Poincare::Internal {
using namespace API;

Layout Regression::equationLayout(
    const double* modelCoefficients, const char* ySymbol, int significantDigits,
    Poincare::Preferences::PrintFloatMode displayMode) const {
  UserExpression formula = expression(modelCoefficients);
  if (formula.isUninitialized()) {
    return Layout();
  }
  UserExpression equation = UserExpression::Create(
      KEqual(KA, KB),
      {.KA = UserExpression::FromSymbol(ySymbol), .KB = formula});
  return equation.createLayout({
      .numberOfSignificantDigits = static_cast<int8_t>(significantDigits),
      .floatMode = displayMode,
  });
}

UserExpression Regression::expression(const double* modelCoefficients) const {
  for (int i = 0; i < numberOfCoefficients(); i++) {
    if (std::isnan(modelCoefficients[i])) {
      return UserExpression();
    }
  }
  return privateExpression(modelCoefficients);
}

double Regression::levelSet(const double* modelCoefficients, double xMin,
                            double xMax, double y,
                            Poincare::Context* context) const {
  UserExpression e = expression(modelCoefficients);
  if (e.isUninitialized()) {
    return NAN;
  }
  Tree* diff = SharedTreeStack->pushAdd(2);
  static_cast<const Tree*>(e)->cloneTree();
  SharedTreeStack->pushFloat(-y);
  Approximation::PrepareFunctionForApproximation(diff, "x",
                                                 ComplexFormat::Real);
  // TODO: use y+evaluate() instead of yTree+e in nextIntersection
  Poincare::Solver solver = Poincare::Solver(xMin, xMax, context);
  solver.stretch();
  double result = solver.nextRoot(diff).x();
  diff->removeTree();
  return result;
}

double Regression::evaluate(const double* modelCoefficients, double x) const {
  Coefficients coefficients;
  memcpy(coefficients.data(), modelCoefficients,
         numberOfCoefficients() * sizeof(double));
  return privateEvaluate(coefficients, x);
}

void Regression::fit(const Series* series, double* modelCoefficients,
                     Poincare::Context* context) const {
  if (!dataSuitableForFit(series)) {
    Coefficients initialCoefficients = initCoefficientsForFit(NAN, true, 0);
    memmove(modelCoefficients, initialCoefficients.data(),
            numberOfCoefficients() * sizeof(double));
    return;
  }
  Coefficients coefficients = privateFit(series, context);
  memmove(modelCoefficients, coefficients.data(),
          numberOfCoefficients() * sizeof(double));
}

Regression::Coefficients Regression::privateFit(
    const Series* series, Poincare::Context* context) const {
  double lowestResidualsSquareSum = OMG::Float::Max<double>();
  std::array<double, k_maxNumberOfCoefficients> bestModelCoefficients;
  /* The coefficients are initialized to zero, so that in the worst case (it
   * could theoretically happen if all residual standard deviations are infinite
   * or NaN), the returned model coefficients will all be zero. */
  bestModelCoefficients.fill(0.0);

  size_t attemptNumber = 0;
  while (attemptNumber < m_initialParametersIterations) {
    Coefficients modelCoefficients = initCoefficientsForFit(
        k_initialCoefficientValue, false, attemptNumber, series);
    fitLevenbergMarquardt(series, modelCoefficients, context);
    uniformizeCoefficientsFromFit(modelCoefficients);
    double newResidualsSquareSum =
        privateResidualsSquareSum(series, modelCoefficients);
    if (isRegressionBetter(newResidualsSquareSum, lowestResidualsSquareSum,
                           modelCoefficients, bestModelCoefficients)) {
      lowestResidualsSquareSum = newResidualsSquareSum;
      bestModelCoefficients = modelCoefficients;
    }
    attemptNumber++;
  }
  return bestModelCoefficients;
}

bool Regression::dataSuitableForFit(const Series* series) const {
  return series->numberOfDistinctAbscissaeGreaterOrEqualTo(
      numberOfCoefficients());
}

void Regression::fitLevenbergMarquardt(const Series* series,
                                       Coefficients& modelCoefficients,
                                       Context* context) const {
  /* We want to find the best coefficients of the regression to minimize the sum
   * of the squares of the difference between a data point and the corresponding
   * point of the fitting regression (chi2 function).
   * We use the Levenberg-Marquardt algorithm to minimize this chi2 merit
   * function.
   * The equation to solve is A'*da = B, with A' a damped version of the chi2
   * Hessian matrix, da the coefficients increments and B colinear to the
   * gradient of chi2.*/
  double currentChi2 = chi2(series, modelCoefficients);
  double lambda = k_initialLambda;
  int n = numberOfCoefficients();  // n unknown coefficients
  int smallChi2ChangeCounts = 0;
  int iterationCount = 0;
  while (smallChi2ChangeCounts < k_consecutiveSmallChi2ChangesLimit &&
         iterationCount < k_maxIterations) {
    // Create the alpha prime matrix (it is symmetric)
    double coefficientsAPrime[Regression::k_maxNumberOfCoefficients *
                              Regression::k_maxNumberOfCoefficients];
    assert(n > 0);  // Ensure that coefficientsAPrime is initialized
    for (int i = 0; i < n; i++) {
      for (int j = i; j < n; j++) {
        double alphaPrime =
            alphaPrimeCoefficient(series, modelCoefficients, i, j, lambda);
        coefficientsAPrime[i * n + j] = alphaPrime;
        if (i != j) {
          coefficientsAPrime[j * n + i] = alphaPrime;
        }
      }
    }
    // Create the beta matrix
    double operandsB[Regression::k_maxNumberOfCoefficients];
    for (int j = 0; j < n; j++) {
      operandsB[j] = betaCoefficient(series, modelCoefficients, j);
    }

    // Compute the equation solution (= vector of coefficients increments)
    double modelCoefficientSteps[Regression::k_maxNumberOfCoefficients];
    if (solveLinearSystem(modelCoefficientSteps, coefficientsAPrime, operandsB,
                          n, context) < 0) {
      break;
    }

    // Compute the new coefficients
    Coefficients newModelCoefficients;
    for (int i = 0; i < n; i++) {
      newModelCoefficients[i] = modelCoefficients[i] + modelCoefficientSteps[i];
    }

    // Compare new chi2 with the previous value
    double newChi2 = chi2(series, newModelCoefficients);
    smallChi2ChangeCounts =
        (fabs(currentChi2 - newChi2) > k_chi2ChangeCondition)
            ? 0
            : smallChi2ChangeCounts + 1;
    if (newChi2 >= currentChi2) {
      lambda *= k_lambdaFactor;
    } else {
      lambda /= k_lambdaFactor;
      for (int i = 0; i < n; i++) {
        modelCoefficients[i] = newModelCoefficients[i];
      }
      currentChi2 = newChi2;
    }
    iterationCount++;
  }
}

double Regression::chi2(const Series* series,
                        const Coefficients& modelCoefficients) const {
  double result = 0.0;
  for (int n = series->numberOfPairs(), i = 0; i < n; i++) {
    double xi = series->getX(i);
    double yi = series->getY(i);
    double difference = yi - privateEvaluate(modelCoefficients, xi);
    result += difference * difference;
  }
  return result;
}

/* a'(k,k) = a(k,k) * (1 + lambda)
 * a'(k,l) = a(l,k) when (k != l) */
double Regression::alphaPrimeCoefficient(const Series* series,
                                         const Coefficients& modelCoefficients,
                                         int k, int l, double lambda) const {
  assert(k >= 0 && k < numberOfCoefficients());
  assert(l >= 0 && l < numberOfCoefficients());
  double result = 0.0;
  if (k == l) {
    /* The Levengerg method uses a'(k,k) = a(k,k) + lambda.
     * The Marquardt method uses a'(k,k) = a(k,k) * (1 + lambda).
     * We use a mixed method to try to make the matrix invertible:
     * a'(k,k) = a(k,k) * (1 + lambda), but if a'(k,k) is too small,
     * a'(k,k) = 2*epsilon so that the inversion method does not detect a'(k,k)
     * as a zero. */
    result = alphaCoefficient(series, modelCoefficients, k, l) * (1.0 + lambda);
    if (std::fabs(result) < OMG::Float::EpsilonLax<double>()) {
      result = 2 * OMG::Float::EpsilonLax<double>();
    }
  } else {
    result = alphaCoefficient(series, modelCoefficients, l, k);
  }
  return result;
}

// a(k,l) = sum(0, N-1, derivate(y(xi|a), ak) * derivate(y(xi|a), a))
double Regression::alphaCoefficient(const Series* series,
                                    const Coefficients& modelCoefficients,
                                    int k, int l) const {
  assert(k >= 0 && k < numberOfCoefficients());
  assert(l >= 0 && l < numberOfCoefficients());
  double result = 0.0;
  for (int n = series->numberOfPairs(), i = 0; i < n; i++) {
    double xi = series->getX(i);
    result += partialDerivate(modelCoefficients, k, xi) *
              partialDerivate(modelCoefficients, l, xi);
  }
  return result;
}

// b(k) = sum(0, N-1, (yi - y(xi|a)) * derivate(y(xi|a), ak))
double Regression::betaCoefficient(const Series* series,
                                   const Coefficients& modelCoefficients,
                                   int k) const {
  assert(k >= 0 && k < numberOfCoefficients());
  double result = 0.0;
  for (int n = series->numberOfPairs(), i = 0; i < n; i++) {
    double xi = series->getX(i);
    double yi = series->getY(i);
    result += (yi - privateEvaluate(modelCoefficients, xi)) *
              partialDerivate(modelCoefficients, k, xi);
  }
  return result;
}

int Regression::solveLinearSystem(double* solutions, double* coefficients,
                                  double* constants, int solutionDimension,
                                  Context* context) const {
  int n = solutionDimension;
  assert(n <= k_maxNumberOfCoefficients);
  double
      coefficientsSave[k_maxNumberOfCoefficients * k_maxNumberOfCoefficients];
  for (int i = 0; i < n * n; i++) {
    coefficientsSave[i] = coefficients[i];
  }
  int inverseResult = MatrixArray::Inverse(coefficients, n, n);
  int numberOfMatrixModifications = 0;
  while (inverseResult < 0 &&
         numberOfMatrixModifications < k_maxMatrixInversionFixIterations) {
    /* If the matrix is not invertible, we modify it to try to make
     * it invertible by multiplying the diagonal coefficients by 1+i/n. This
     * will change the iterative path of the algorithm towards the chi2 minimum,
     * but not the final solution itself, as the stopping condition is that chi2
     * is at its minimum, so when B is null. */
    for (int i = 0; i < n; i++) {
      coefficientsSave[i * n + i] =
          (1 + ((double)i) / ((double)n)) * coefficientsSave[i * n + i];
    }
    inverseResult = MatrixArray::Inverse(coefficientsSave, n, n);
    numberOfMatrixModifications++;
  }
  if (inverseResult < 0) {
    return -1;
  }
  if (numberOfMatrixModifications > 0) {
    for (int i = 0; i < n * n; i++) {
      coefficients[i] = coefficientsSave[i];
    }
  }
  MatrixArray::Multiplication<double>(coefficients, constants, solutions, n, n,
                                      1);
  return 0;
}

Regression::Coefficients Regression::initCoefficientsForFit(
    double defaultValue, bool forceDefaultValue, size_t attemptNumber,
    const Series* series) const {
  return forceDefaultValue ? Regression::specializedInitCoefficientsForFit(
                                 defaultValue, attemptNumber)
                           : specializedInitCoefficientsForFit(
                                 defaultValue, attemptNumber, series);
}

Regression::Coefficients Regression::specializedInitCoefficientsForFit(
    double defaultValue, size_t /* attemptNumber */,
    const Series* /* series */) const {
  Coefficients coefficients;
  coefficients.fill(defaultValue);
  return coefficients;
}

double Regression::correlationCoefficient(const Series* series) const {
  /* Returns the correlation coefficient (R) between the series X and Y
   * (transformed if series is a TransformedModel). In non-linear and
   * non-transformed regressions, its square is different from the
   * determinationCoefficient R2. it is then hidden to avoid confusion */
  const Type thisType = type();
  if (!HasR(thisType)) {
    return NAN;
  }
  bool applyLn = FitsLnY(thisType);
  bool applyOpposite = applyLn && series->getY(0) < 0.0;
  StatisticsCalculationOptions options(FitsLnX(thisType), applyLn,
                                       applyOpposite);

  double v0 = series->createDatasetFromColumn(0, options).variance();
  double v1 = series->createDatasetFromColumn(1, options).variance();
  if (std::isnan(v0) || std::isnan(v1)) {
    // Can happen if applyLn on negative/null values
    return NAN;
  }
  /* Compare v0 and v1 to EpsilonLax to check if they are equal to zero (since
   * approximation errors could give them > 0 while they are not.)*/
  double result = (std::abs(v0) < OMG::Float::EpsilonLax<double>() ||
                   std::abs(v1) < OMG::Float::EpsilonLax<double>())
                      ? 1.0
                      : series->covariance(options) / std::sqrt(v0 * v1);
  /* Due to errors, coefficient could slightly exceed 1.0. It needs to be
   * fixed here to prevent r^2 from being bigger than 1. */
  if (std::abs(result) <= 1.0 + OMG::Float::SqrtEpsilonLax<double>()) {
    return std::clamp(result, -1.0, 1.0);
  }
  return NAN;
}

double Regression::determinationCoefficient(
    const Series* series, const double* modelCoefficients) const {
  // Computes and returns the determination coefficient of the regression.
  const Type thisType = type();
  if (HasRSquared(thisType)) {
    /* With linear regressions and transformed models (Exponential, Logarithm
     * and Power), we use r^2, the square of the correlation coefficient between
     * the series Y (transformed) and the evaluated values.*/
    double r = correlationCoefficient(series);
    return r * r;
  }

  if (!HasR2(thisType)) {
    /* R2 does not need to be computed if model is median-median, so we avoid
     * computation. If needed, it could be computed though. */
    return NAN;
  }

  assert(!FitsLnY(thisType) && !FitsLnX(thisType));
  /* With proportional regression or badly fitted models, R2 can technically be
   * negative. R2<0 means that the regression is less effective than a
   * constant set to the series average. It should not happen with regression
   * models that can fit a constant observation. */
  // Residual sum of squares
  double ssr = 0;
  // Total sum of squares
  double sst = 0;
  const int numberOfPairs = series->numberOfPairs();
  assert(numberOfPairs > 0);
  double mean = series->createDatasetFromColumn(1).mean();
  for (int k = 0; k < numberOfPairs; k++) {
    // Difference between the observation and the estimated value of the model
    double estimation = evaluate(modelCoefficients, series->getX(k));
    double observation = series->getY(k);
    if (std::isnan(estimation) || std::isinf(estimation)) {
      // Data Not Suitable for estimation
      return NAN;
    }
    double residual = observation - estimation;
    ssr += residual * residual;
    // Difference between the observation and the overall observations mean
    double difference = observation - mean;
    sst += difference * difference;
  }
  if (sst == 0.0) {
    /* Observation was constant, r2 is undefined. Return 1 if estimations
     * exactly matched observations. 0 is usually returned otherwise. */
    return (ssr <= DBL_EPSILON) ? 1.0 : 0.0;
  }
  double r2 = 1.0 - ssr / sst;
  /* Check if regression fit was optimal.
   * TODO: Optimize regression fitting so that r2 cannot be negative.
   * assert(r2 >= 0 || seriesRegressionType(series) ==
   * Model::Type::Proportional); */
  return r2;
}

double Regression::residualAtIndex(const Series* series,
                                   const double* modelCoefficients,
                                   int index) const {
  return series->getY(index) - evaluate(modelCoefficients, series->getX(index));
}

double Regression::residualStandardDeviation(
    const Series* series, const double* modelCoefficients) const {
  Coefficients coefficients;
  memcpy(coefficients.data(), modelCoefficients,
         numberOfCoefficients() * sizeof(double));
  return privateResidualStandardDeviation(series, coefficients);
}

double Regression::privateResidualAtIndex(const Series* series,
                                          const Coefficients& modelCoefficients,
                                          int index) const {
  return series->getY(index) -
         privateEvaluate(modelCoefficients, series->getX(index));
}

double Regression::privateResidualsSquareSum(
    const Series* series, const Coefficients& modelCoefficients) const {
  int n = series->numberOfPairs();
  double sum = 0.;
  for (int i = 0; i < n; i++) {
    double res = privateResidualAtIndex(series, modelCoefficients, i);
    sum += res * res;
  }
  return sum;
}

double Regression::privateResidualStandardDeviation(
    const Series* series, const Coefficients& modelCoefficients) const {
  int nCoeff = numberOfCoefficients();
  int n = series->numberOfPairs();
  if (n <= nCoeff) {
    return NAN;
  }
  double sum = privateResidualsSquareSum(series, modelCoefficients);
  return std::sqrt(sum / (n - nCoeff));
}

}  // namespace Poincare::Internal
