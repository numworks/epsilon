#ifndef POINCARE_SOLVER_ALGORITHMS_H
#define POINCARE_SOLVER_ALGORITHMS_H

#include <poincare/solver2.h>

namespace Poincare {

class SolverAlgorithms {
public:
  static Coordinate2D<double> BrentRoot(Solver2<double>::FunctionEvaluation f, const void * aux, double xMin, double xMax, Solver2<double>::Interest interest, double precision);
  /* BrentMinimum
   * IncreasingFunctionRoot
   * CumulativeFrequency
   * CumulativeInverse
   */
};

}

#endif
