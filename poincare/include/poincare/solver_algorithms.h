#ifndef POINCARE_SOLVER_ALGORITHMS_H
#define POINCARE_SOLVER_ALGORITHMS_H

#include <poincare/solver2.h>

namespace Poincare {

class SolverAlgorithms {
public:
  /* Bibliography: R. P. Brent, Algorithms for finding zeros and extrema of
   * functions without calculating derivatives */
  static Coordinate2D<double> BrentRoot(Solver2<double>::FunctionEvaluation f, const void * aux, double xMin, double xMax, Solver2<double>::Interest interest, double precision);
  static Coordinate2D<double> BrentMinimum(Solver2<double>::FunctionEvaluation f, const void * aux, double xMin, double xMax, Solver2<double>::Interest interest, double precision);

  /* IncreasingFunctionRoot
   * CumulativeFrequency
   * CumulativeInverse
   */

private:
  constexpr static int k_numberOfIterationsBrent = 100;
  constexpr static double k_sqrtEps = 1.4901161193847656E-8; // sqrt(DBL_EPSILON)
  constexpr static double k_goldenRatio = 0.381966011250105151795413165634361882279690820194237137864; // (3-sqrt(5))/2
};

}

#endif
