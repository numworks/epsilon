#ifndef APPS_PROBABILITY_MODELS_LAW_HELPER_H
#define APPS_PROBABILITY_MODELS_LAW_HELPER_H

#include <poincare/solver.h>

namespace Probability {

template <typename T>
void findBoundsForBinarySearch(Poincare::Solver::ValueAtAbscissa evaluation,
                Poincare::Context * context,
                const void * auxiliary,
                T & xmin,
                T & xmax) {
  constexpr static int k_maxNumberOfIterations = 1e9;

  xmin = 0, xmax = 10;
  T sign = evaluation(0, context, auxiliary) < 0 ? 1 : -1;
  int iteration = 0;

  while ((sign * evaluation(sign * xmax, context, auxiliary) < 0) &&
         (iteration < k_maxNumberOfIterations)) {
    xmin = xmax;
    xmax *= 10;
    iteration++;
  }
  // Enlarge interval to avoid bounds being too close to solution
  xmax += 1;
  xmin -= 1;

  if (iteration == k_maxNumberOfIterations) {
    // Failed to find zero
    assert(false);
    xmin = NAN;
    xmax = NAN;
  }

  if (sign < 0) {
    double temp = -xmin;
    xmin = -xmax;
    xmax = temp;
  }
}

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_LAW_HELPER_H */
