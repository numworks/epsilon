#include <poincare/binomial_distribution.h>
#include <poincare/chi2_distribution.h>
#include <poincare/distribution.h>
#include <poincare/exponential_distribution.h>
#include <poincare/fisher_distribution.h>
#include <poincare/geometric_distribution.h>
#include <poincare/hypergeometric_distribution.h>
#include <poincare/normal_distribution.h>
#include <poincare/poisson_distribution.h>
#include <poincare/solver_algorithms.h>
#include <poincare/student_distribution.h>
#include <poincare/uniform_distribution.h>

namespace Poincare {

const Distribution *Distribution::Get(Type type) {
  switch (type) {
    case Type::Binomial:
      constexpr static BinomialDistribution binomial;
      return &binomial;
    case Type::Normal:
      constexpr static NormalDistribution normal;
      return &normal;
    case Type::Student:
      constexpr static StudentDistribution student;
      return &student;
    case Type::Geometric:
      constexpr static GeometricDistribution geometric;
      return &geometric;
    case Type::Hypergeometric:
      constexpr static HypergeometricDistribution hypergeometric;
      return &hypergeometric;
    case Type::Fisher:
      constexpr static FisherDistribution fisher;
      return &fisher;
    case Type::Uniform:
      constexpr static UniformDistribution uniform;
      return &uniform;
    case Type::Exponential:
      constexpr static ExponentialDistribution exponential;
      return &exponential;
    case Type::Poisson:
      constexpr static PoissonDistribution poisson;
      return &poisson;
    default:
      assert(type == Type::ChiSquared);
      constexpr static Chi2Distribution chiSquared;
      return &chiSquared;
  }
}

template <typename T>
void Distribution::findBoundsForBinarySearch(
    typename Solver<T>::FunctionEvaluation cumulativeDistributionEvaluation,
    const void *auxiliary, T &xmin, T &xmax) {
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
  xmin -= 1;

  assert(iteration != k_maxNumberOfIterations);

  if (signOfRoot < 0) {
    T temp = -xmin;
    xmin = -xmax;
    xmax = temp;
  }
}

double Distribution::
    cumulativeDistributiveInverseForProbabilityUsingIncreasingFunctionRoot(
        double p, double ax, double bx, double *parameters) const {
  assert(ax < bx);
  if (p > 1.0 - DBL_EPSILON) {
    return INFINITY;
  }
  if (p < DBL_EPSILON) {
    return -INFINITY;
  }
  const void *pack[3] = {this, &p, parameters};
  Coordinate2D<double> result = SolverAlgorithms::IncreasingFunctionRoot(
      ax, bx, DBL_EPSILON,
      [](double x, const void *auxiliary) {
        const void *const *pack = static_cast<const void *const *>(auxiliary);
        const Distribution *distribution =
            static_cast<const Distribution *>(pack[0]);
        const double *proba = static_cast<const double *>(pack[1]);
        const double *parameters = static_cast<const double *>(pack[2]);
        // This needs to be an increasing function
        return distribution->cumulativeDistributiveFunctionAtAbscissa(
                   x, parameters) -
               *proba;
      },
      pack);
  /* Either no result was found, the precision is ok or the result was outside
   * the given ax bx bounds */
  if (!(std::isnan(result.x2()) || std::fabs(result.x2()) <= FLT_EPSILON ||
        std::fabs(result.x1() - ax) < FLT_EPSILON ||
        std::fabs(result.x1() - bx) < FLT_EPSILON)) {
    /* We would like to put this as an assertion, but sometimes we do get
     * false result: we replace them with inf to make the problem obvious to
     * the student. */
    assert(
        false);  // TODO this assert is used to hunt a case where that happens.
                 // If it doesn't, then we can remove this block of code
    return p > 0.5 ? INFINITY : -INFINITY;
  }
  return result.x1();
}

template void Distribution::findBoundsForBinarySearch<float>(
    Solver<float>::FunctionEvaluation, void const *, float &, float &);
template void Distribution::findBoundsForBinarySearch<double>(
    Solver<double>::FunctionEvaluation, void const *, double &, double &);

}  // namespace Poincare
