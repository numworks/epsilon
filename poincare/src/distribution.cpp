#include <poincare/distribution.h>
#include <poincare/binomial_distribution.h>
#include <poincare/normal_distribution.h>
#include <poincare/student_distribution.h>
#include <poincare/geometric_distribution.h>
#include <poincare/exponential_distribution.h>
#include <poincare/poisson_distribution.h>
#include <poincare/fisher_distribution.h>
#include <poincare/uniform_distribution.h>
#include <poincare/chi2_distribution.h>

namespace Poincare {

const Distribution * Distribution::Get(Type type) {
  switch (type) {
  case Type::Binomial:
    static constexpr BinomialDistribution binomial;
    return &binomial;
  case Type::Normal:
    static constexpr NormalDistribution normal;
    return &normal;
  case Type::Student:
    static constexpr StudentDistribution student;
    return &student;
  case Type::Geometric:
    static constexpr GeometricDistribution geometric;
    return &geometric;
  case Type::Fisher:
    static constexpr FisherDistribution fisher;
    return &fisher;
  case Type::Uniform:
    static constexpr UniformDistribution uniform;
    return &uniform;
  case Type::Exponential:
    static constexpr ExponentialDistribution exponential;
    return &exponential;
  case Type::Poisson:
    static constexpr PoissonDistribution poisson;
    return &poisson;
  case Type::ChiSquared:
    static constexpr Chi2Distribution chiSquared;
    return &chiSquared;
  }
}

template <typename T> void Distribution::findBoundsForBinarySearch(Poincare::Solver::ValueAtAbscissa cumulativeDistributionEvaluation, Poincare::Context * context, const void * auxiliary, T & xmin, T & xmax) {
  /* We'll simply test [0, 10], [10, 100], [100, 1000] ... until we find a working interval, or
   * symmetrically if the zero is on the left. This obviously assumes that
   * cumulativeDistributionEvaluation is an increasing function.*/
  constexpr static int k_maxNumberOfIterations = 308;  // std::log10(DBL_MAX)

  xmin = 0, xmax = 10;
  T signOfRoot = cumulativeDistributionEvaluation(0, context, auxiliary) < 0 ? 1 : -1;
  int iteration = 0;

  // We check if xmax if after the root, and otherwise multiply it by 10
  while ((signOfRoot * cumulativeDistributionEvaluation(signOfRoot * xmax, context, auxiliary) < 0) &&
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

double Distribution::cumulativeDistributiveInverseForProbabilityUsingIncreasingFunctionRoot(double p, double ax, double bx, double * parameters) const {
  assert(ax < bx);
  if (p > 1.0 - DBL_EPSILON) {
    return INFINITY;
  }
  if (p < DBL_EPSILON) {
    return -INFINITY;
  }
  const void * pack[3] = { this, &p, parameters };
  Coordinate2D<double> result = Solver::IncreasingFunctionRoot(
      ax, bx, DBL_EPSILON,
      [](double x, Poincare::Context * context, const void * auxiliary) {
        const void * const * pack = static_cast<const void * const *>(auxiliary);
        const Distribution * distribution = static_cast<const Distribution *>(pack[0]);
        const double * proba = static_cast<const double *>(pack[1]);
        const double * parameters = static_cast<const double *>(pack[2]);
        return distribution->CumulativeDistributiveFunctionAtAbscissa(x, parameters) - *proba; // This needs to be an increasing function
      },
      nullptr, pack);
  /* Either no result was found, the precision is ok or the result was outside
   * the given ax bx bounds */
   if (!(std::isnan(result.x2()) || std::fabs(result.x2()) <= FLT_EPSILON || std::fabs(result.x1()- ax) < FLT_EPSILON || std::fabs(result.x1() - bx) < FLT_EPSILON)) {
     /* We would like to put this as an assertion, but sometimes we do get
      * false result: we replace them with inf to make the problem obvious to
      * the student. */
     assert(false);  // TODO this assert is used to hunt a case where that happens. If it doesn't, then we can remove this block of code
     return p > 0.5 ? INFINITY : -INFINITY;
   }
   return result.x1();
}

template void Distribution::findBoundsForBinarySearch<float>(double (*)(double, Poincare::Context*, void const*), Poincare::Context*, void const*, float&, float&);
template void Distribution::findBoundsForBinarySearch<double>(double (*)(double, Poincare::Context*, void const*), Poincare::Context*, void const*, double&, double&);

}
