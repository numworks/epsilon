#ifndef POINCARE_DISTRIBUTION_H
#define POINCARE_DISTRIBUTION_H

#include <poincare/solver.h>

namespace Poincare {

/* This method looks for bounds such that:
 * cumulativeDistributionEvaluation(xmin) < 0 < cumulativeDistributionEvaluation(xmax)
 */

class Distribution {
protected:
  template <typename T> static void findBoundsForBinarySearch(Poincare::Solver::ValueAtAbscissa cumulativeDistributionEvaluation, Poincare::Context * context, const void * auxiliary, T & xmin, T & xmax);
};

}

#endif
