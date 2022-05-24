#include <poincare/chi2_distribution.h>
#include <poincare/rational.h>
#include <poincare/regularized_gamma_function.h>
#include <cmath>

namespace Poincare {

template <typename T>
T Chi2Distribution::EvaluateAtAbscissa(T x, T k) {
  if (x < 0.0) {
    return NAN;
  }
  if (x == 0.0) {
    return 0.0;
  }
  if (std::isinf(x)) {
    return 0.0;
  }
  const T halfk = k / 2.0;
  const T halfX = x / 2.0;
  return std::exp(-lgamma(halfk) - halfX + (halfk - 1.0) * std::log(halfX)) / 2.0;
}

template <typename T>
T Chi2Distribution::CumulativeDistributiveFunctionAtAbscissa(T x, T k) {
  if (x < DBL_EPSILON) {
    return 0.0;
  }
  double result = 0.0;
  if (RegularizedGammaFunction(k / 2.0,
                       x / 2.0,
                       k_regularizedGammaPrecision,
                       k_maxRegularizedGammaIterations,
                       &result)) {
    return result;
  }
  return NAN;
}

template <typename T>
T Chi2Distribution::CumulativeDistributiveInverseForProbability(T probability, T k) {
  // Compute inverse using Solver::IncreasingFunctionRoot
  if (probability > 1.0 - DBL_EPSILON) {
    return INFINITY;
  } else if (probability < DBL_EPSILON) {
    return 0;
  }

  struct Args {
    T proba;
    T k;
  };
  Args args{probability, k};

  Poincare::Solver::ValueAtAbscissa evaluation =
      [](double x, Poincare::Context * context, const void * auxiliary) {
        const Args * args = static_cast<const Args *>(auxiliary);
        return CumulativeDistributiveFunctionAtAbscissa<double>(x, args->k) - args->proba;
      };

  double xmin, xmax;  // IncreasingFunctionRoot requires double
  findBoundsForBinarySearch(evaluation, nullptr, &args, xmin, xmax);

  Poincare::Coordinate2D<double> result =
      Poincare::Solver::IncreasingFunctionRoot(xmin, xmax, DBL_EPSILON, evaluation, nullptr, &args);
  return result.x1();
}

template<typename T>
bool Chi2Distribution::KIsOK(T k) {
  return !std::isnan(k) && !std::isinf(k)
    && k > static_cast<T>(DBL_EPSILON);
}

bool Chi2Distribution::ExpressionKIsOK(bool * result, const Expression & k, Context * context) {
  assert(result != nullptr);
  if (k.deepIsMatrix(context)) {
    *result = false;
    return true;
  }

  if (k.isUndefined() || Expression::IsInfinity(k, context)) {
    // TODO : use normal distribution when k = +inf
    *result = false;
    return true;
  }
  if (!k.isReal(context)) {
    // We cannot check that k is real
    return false;
  }

  {
    ExpressionNode::Sign s = k.sign(context);
    if (s == ExpressionNode::Sign::Negative) {
      *result = false;
      return true;
    }
    // We cannot check that k is positive
    if (s != ExpressionNode::Sign::Positive) {
      return false;
    }
  }

  if (k.type() != ExpressionNode::Type::Rational) {
    // We cannot check that k is not null
    return false;
  }

  const Rational rationalVar = static_cast<const Rational &>(k);
  if (rationalVar.isZero()) {
    *result = false;
    return true;
    }
  *result = true;
  return true;
}

// Specialisations
template float Chi2Distribution::EvaluateAtAbscissa<float>(float, float);
template double Chi2Distribution::EvaluateAtAbscissa<double>(double, double);
template float Chi2Distribution::CumulativeDistributiveFunctionAtAbscissa<float>(float, float);
template double Chi2Distribution::CumulativeDistributiveFunctionAtAbscissa<double>(double, double);
template float Chi2Distribution::CumulativeDistributiveInverseForProbability<float>(float, float);
template double Chi2Distribution::CumulativeDistributiveInverseForProbability<double>(double, double);
template bool Chi2Distribution::KIsOK(float k);
template bool Chi2Distribution::KIsOK(double k);

}
