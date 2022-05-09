#include <poincare/distribution.h>
#include <poincare/binomial_distribution.h>
#include <poincare/normal_distribution.h>
#include <poincare/student_distribution.h>
#include <poincare/geometric_distribution.h>
#include <poincare/poisson_distribution.h>
#include <new>

namespace Poincare {

Distribution * Distribution::Get(Type type) {
  switch (type) {
  case Type::Binomial:
    static BinomialDistribution binomial;
    return &binomial;
  case Type::Normal:
    static NormalDistribution normal;
    return &normal;
  case Type::Student:
    static StudentDistribution student;
    return &student;
  case Type::Geometric:
    static GeometricDistribution geometric;
    return &geometric;
  case Type::Poisson:
    static PoissonDistribution poisson;
    return &poisson;
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

template void Distribution::findBoundsForBinarySearch<float>(double (*)(double, Poincare::Context*, void const*), Poincare::Context*, void const*, float&, float&);
template void Distribution::findBoundsForBinarySearch<double>(double (*)(double, Poincare::Context*, void const*), Poincare::Context*, void const*, double&, double&);

}
