#include <assert.h>
#include <omg/float.h>
#include <omg/troolean.h>
#include <omg/unreachable.h>
#include <poincare/src/solver/beta_function.h>
#include <poincare/src/solver/erf_inv.h>
#include <poincare/src/solver/regularized_incomplete_beta_function.h>
#include <poincare/src/solver/solver_algorithms.h>
#include <poincare/statistics/distribution.h>

namespace Poincare::Internal::Distribution {

template <typename T>
void findBoundsForBinarySearch(
    typename Solver<T>::FunctionEvaluation cumulativeDistributionEvaluation,
    const void* auxiliary, T& xmin, T& xmax) {
  /* We'll simply test [0, 10], [10, 100], [100, 1000] ... until we find a
   * working interval, or symmetrically if the zero is on the left. This
   * obviously assumes that cumulativeDistributionEvaluation is an increasing
   * function.*/
  constexpr static int k_maxNumberOfIterations = 308;  // std::log10(DBL_MAX)

  xmin = 0, xmax = 10;
  T signOfRoot = cumulativeDistributionEvaluation(0, auxiliary) < 0 ? 1 : -1;
  int iteration = 0;

  // We check if xmax if after the root, and otherwise multiply it by 10
  while ((signOfRoot *
              cumulativeDistributionEvaluation(signOfRoot * xmax, auxiliary) <
          0) &&
         (iteration < k_maxNumberOfIterations)) {
    xmin = xmax;
    xmax *= 10;
    iteration++;
  }
  // Enlarge interval to avoid bounds being too close to solution
  xmax += 1;
  xmin = std::max(static_cast<T>(0), xmin - 1);

  assert(iteration != k_maxNumberOfIterations);

  if (signOfRoot < 0) {
    T temp = -xmin;
    xmin = -xmax;
    xmax = temp;
  }
}

template <typename T>
T binomialCumulativeDistributiveInverse(
    T probability, const Distribution::ParametersArray<T> parameters) {
  const T n = parameters[Params::Binomial::N];
  const T p = parameters[Params::Binomial::P];
  constexpr T precision = OMG::Float::Epsilon<T>();
  bool nIsZero = std::abs(n) < precision;
  bool pIsZero = std::abs(p) < precision;
  bool pIsOne = !pIsZero && std::abs(p - static_cast<T>(1.0)) < precision;
  if (nIsZero && (pIsZero || pIsOne)) {
    return NAN;
  }
  if (std::abs(probability) < precision) {
    if (pIsOne) {
      return 0;
    }
    return NAN;
  }
  if (std::abs(probability - static_cast<T>(1.0)) < precision) {
    return n;
  }
  T proba = probability;
  return SolverAlgorithms::CumulativeDistributiveInverseForNDefinedFunction<T>(
      &proba,
      [](T x, const void* auxiliary) {
        const Distribution::ParametersArray<T>* params =
            reinterpret_cast<const Distribution::ParametersArray<T>*>(
                auxiliary);
        return EvaluateAtAbscissa(Type::Binomial, x, *params);
      },
      &parameters);
}

template <typename T>
T chi2CumulativeDistributiveInverse(
    T probability, const Distribution::ParametersArray<T> parameters) {
  // Compute inverse using SolverAlgorithms::IncreasingFunctionRoot
  if (probability > 1.0 - DBL_EPSILON) {
    return INFINITY;
  } else if (probability < DBL_EPSILON) {
    return 0.;
  }

  const T k = parameters[0];

  struct Args {
    T proba;
    T k;
  };
  Args args{probability, k};

  Solver<double>::FunctionEvaluation evaluation = [](double x,
                                                     const void* auxiliary) {
    const Args* args = static_cast<const Args*>(auxiliary);
    double dblK = static_cast<double>(args->k);
    return CumulativeDistributiveFunctionAtAbscissa<double>(
               Type::Chi2, x, Distribution::ParametersArray<double>({dblK})) -
           args->proba;
  };

  double xmin, xmax;
  findBoundsForBinarySearch(evaluation, &args, xmin, xmax);

  Coordinate2D<double> result = SolverAlgorithms::IncreasingFunctionRoot(
      xmin, xmax, DBL_EPSILON, evaluation, &args);
  return static_cast<T>(result.x());
}

template <typename T>
T exponentialCumulativeDistributiveInverse(
    T probability, const Distribution::ParametersArray<T> params) {
  const T lambda = params[0];
  return -std::log(1.0 - probability) / lambda;
}

template <typename T>
T fischerCumulativeDistributiveInverse(
    T probability, const Distribution::ParametersArray<T> params) {
  const T d1 = params[Params::Fisher::D1];
  const T d2 = params[Params::Fisher::D2];
  const Distribution::ParametersArray<double> dbleParameters(
      {static_cast<double>(d1), static_cast<double>(d2)});
  const double p = static_cast<double>(probability);

  if (p > 1.0 - DBL_EPSILON) {
    return INFINITY;
  }
  if (p < DBL_EPSILON) {
    return -INFINITY;
  }
  double ax = DBL_EPSILON;
  double bx = 100.0;  // Arbitrary value
  const void* pack[2] = {&p, &dbleParameters};
  Coordinate2D<double> result = SolverAlgorithms::IncreasingFunctionRoot(
      ax, bx, DBL_EPSILON,
      [](double x, const void* auxiliary) {
        const void* const* pack = static_cast<const void* const*>(auxiliary);
        const double* proba = static_cast<const double*>(pack[0]);
        const Distribution::ParametersArray<double>* parameters =
            static_cast<const Distribution::ParametersArray<double>*>(pack[1]);
        // This needs to be an increasing function
        return CumulativeDistributiveFunctionAtAbscissa(Type::Fisher, x,
                                                        *parameters) -
               *proba;
      },
      pack);
  /* Either no result was found, the precision is ok or the result was outside
   * the given ax bx bounds */
  if (!(std::isnan(result.y()) || std::fabs(result.y()) <= FLT_EPSILON ||
        std::fabs(result.x() - ax) < FLT_EPSILON ||
        std::fabs(result.x() - bx) < FLT_EPSILON)) {
    /* We would like to put this as an assertion, but sometimes we do get
     * false result: we replace them with inf to make the problem obvious to
     * the student.
     * EXAMPLE: Fisher law, d1=2, d2=2.2*10^-16, try to find P(X<=a) = 0.25
     *
     * TODO: Find a better way to display that no solution could be found. */
    return p > 0.5 ? INFINITY : -INFINITY;
  }
  return result.x();
}

template <typename T>
T geometricCumulativeDistributiveInverse(
    T probability, const Distribution::ParametersArray<T> params) {
  constexpr T precision = OMG::Float::Epsilon<T>();
  if (std::abs(probability) < precision) {
    return NAN;
  }
  const T p = params[0];
  if (std::abs(probability - static_cast<T>(1.0)) < precision) {
    if (std::abs(p - static_cast<T>(1.0)) < precision) {
      return static_cast<T>(1.0);
    }
    return INFINITY;
  }
  T proba = probability;
  /* It works even if G(p) is defined on N* and not N because G(0) returns 0 and
   * not undef */
  return SolverAlgorithms::CumulativeDistributiveInverseForNDefinedFunction<T>(
      &proba,
      [](T x, const void* auxiliary) {
        return EvaluateAtAbscissa(
            Type::Geometric, x,
            *static_cast<const Distribution::ParametersArray<T>*>(auxiliary));
      },
      &params);
}

template <typename T>
T hypergeomCumulativeDistributiveInverse(
    T probability, const Distribution::ParametersArray<T> parameters) {
  const T N = parameters[Params::Hypergeometric::NPop];
  const T K = parameters[Params::Hypergeometric::K];
  const T n = parameters[Params::Hypergeometric::NSample];

  constexpr T precision = OMG::Float::Epsilon<T>();
  if (probability < precision) {
    // We can have 0 successes only if there are enough failures
    if (n > N - K) {
      return 0;
    }
    return NAN;
  }
  if (1.0 - probability < precision) {
    return std::min(n, K);
  }
  T proba = probability;
  return SolverAlgorithms::CumulativeDistributiveInverseForNDefinedFunction<T>(
      &proba,
      [](T x, const void* auxiliary) {
        const Distribution::ParametersArray<T>* params =
            static_cast<const Distribution::ParametersArray<T>*>(auxiliary);
        return EvaluateAtAbscissa(Type::Hypergeometric, x, *params);
      },
      &parameters);
}

template <typename T>
static T standardNormalCumulativeDistributiveInverse(T probability) {
  if (probability > static_cast<T>(1.0) || probability < static_cast<T>(0.0) ||
      !std::isfinite(probability)) {
    return NAN;
  }
  constexpr T precision = OMG::Float::Epsilon<T>();
  if ((static_cast<T>(1.0)) - probability < precision) {
    return INFINITY;
  }
  if (probability < precision) {
    return -INFINITY;
  }
  if (probability < static_cast<T>(0.5)) {
    return -standardNormalCumulativeDistributiveInverse((static_cast<T>(1.0)) -
                                                        probability);
  }
  return static_cast<T>(M_SQRT2) *
         erfInv((static_cast<T>(2.0)) * probability - static_cast<T>(1.0));
}

template <typename T>
T normalCumulativeDistributiveInverse(
    T probability, const Distribution::ParametersArray<T> params) {
  const T mu = params[Params::Normal::Mu];
  const T sigma = params[Params::Normal::Sigma];
  return standardNormalCumulativeDistributiveInverse(probability) *
             std::fabs(sigma) +
         mu;
}

template <typename T>
T poissonCumulativeDistributiveInverse(
    T probability, const Distribution::ParametersArray<T> parameters) {
  constexpr T precision = OMG::Float::Epsilon<T>();
  if (std::abs(probability) < precision) {
    return NAN;
  }
  if (std::abs(probability - static_cast<T>(1.0)) < precision) {
    return INFINITY;
  }
  T proba = probability;
  return SolverAlgorithms::CumulativeDistributiveInverseForNDefinedFunction<T>(
      &proba,
      [](T x, const void* auxiliary) {
        const Distribution::ParametersArray<T>* params =
            static_cast<const Distribution::ParametersArray<T>*>(auxiliary);
        return EvaluateAtAbscissa(Type::Poisson, x, *params);
      },
      &parameters);
}

template <typename T>
T studentCumulativeDistributiveInverse(
    T probability, const Distribution::ParametersArray<T> params) {
  if (probability == 0.5) {
    return static_cast<T>(0.0);
  } else if (probability > 1.0 - DBL_EPSILON) {
    return INFINITY;
  } else if (probability < DBL_EPSILON) {
    return -INFINITY;
  }
  const T k = params[0];
  struct Args {
    T proba;
    T k;
  };
  Args args{probability, k};
  Solver<double>::FunctionEvaluation evaluation = [](double x,
                                                     const void* auxiliary) {
    const Args* args = static_cast<const Args*>(auxiliary);
    const T k = args->k;
    return static_cast<double>(
        CumulativeDistributiveFunctionAtAbscissa<T>(
            Type::Student, x, Distribution::ParametersArray<T>({k})) -
        args->proba);
  };

  double xmin, xmax;
  findBoundsForBinarySearch(evaluation, &args, xmin, xmax);
  assert((xmin < xmax) && std::isfinite(xmin) && std::isfinite(xmax));

  // Compute inverse using SolverAlgorithms::IncreasingFunctionRoot
  Coordinate2D<double> result = SolverAlgorithms::IncreasingFunctionRoot(
      xmin, xmax, DBL_EPSILON, evaluation, &args);
  return result.x();
}

template <typename T>
T uniformCumulativeDistributiveInverse(
    T probability, const Distribution::ParametersArray<T> params) {
  const T a = params[Params::Uniform::A];
  const T b = params[Params::Uniform::B];
  if (probability >= 1.0f) {
    return b;
  }
  if (probability <= 0.0f) {
    return a;
  }
  return a * (1 - probability) + probability * b;
}

template <typename T>
T CumulativeDistributiveInverseForProbability(
    Type type, T probability, const ParametersArray<T> parameters) {
  if (AreParametersValid(type, parameters) != OMG::Troolean::True ||
      std::isnan(probability) || probability < static_cast<T>(0.0) ||
      probability > static_cast<T>(1.0)) {
    return NAN;
  }
  switch (type) {
    case Type::Binomial:
      return binomialCumulativeDistributiveInverse(probability, parameters);
    case Type::Uniform:
      return uniformCumulativeDistributiveInverse(probability, parameters);
    case Type::Exponential:
      return exponentialCumulativeDistributiveInverse(probability, parameters);
    case Type::Normal:
      return normalCumulativeDistributiveInverse(probability, parameters);
    case Type::Chi2:
      return chi2CumulativeDistributiveInverse(probability, parameters);
    case Type::Student:
      return studentCumulativeDistributiveInverse(probability, parameters);
    case Type::Geometric:
      return geometricCumulativeDistributiveInverse(probability, parameters);
    case Type::Hypergeometric:
      return hypergeomCumulativeDistributiveInverse(probability, parameters);
    case Type::Poisson:
      return poissonCumulativeDistributiveInverse(probability, parameters);
    case Type::Fisher:
      return fischerCumulativeDistributiveInverse(probability, parameters);
    default:
      OMG::unreachable();
  }
}

template float CumulativeDistributiveInverseForProbability(
    Type type, float probability,
    const Distribution::ParametersArray<float> parameters);
template double CumulativeDistributiveInverseForProbability(
    Type type, double probability,
    const Distribution::ParametersArray<double> parameters);

}  // namespace Poincare::Internal::Distribution
