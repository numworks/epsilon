#ifndef PROBABILITY_MODELS_LAW_HELPER_H
#define PROBABILITY_MODELS_LAW_HELPER_H

#include <poincare/solver.h>

namespace Probability {

/* This method looks for bounds such that cumulativeDistributionEvaluation(xmin) < 0 <
 * cumulativeDistributionEvaluation(xmax). */
template <typename T>
void findBoundsForBinarySearch(Poincare::Solver::ValueAtAbscissa cumulativeDistributionEvaluation,
                               Poincare::Context * context,
                               const void * auxiliary,
                               T & xmin,
                               T & xmax) {
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
    double temp = -xmin;
    xmin = -xmax;
    xmax = temp;
  }
}

}  // namespace Probability

#endif /* PROBABILITY_MODELS_LAW_HELPER_H */
