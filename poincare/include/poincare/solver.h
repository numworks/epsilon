#ifndef POINCARE_SOLVER_H
#define POINCARE_SOLVER_H

#include <poincare/context.h>
#include <poincare/coordinate_2D.h>
#include <poincare/preferences.h>

namespace Poincare {

class Solver {
public:
  typedef double (*ValueAtAbscissa)(double abscissa, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const void * context1, const void * context2, const void * context3);
  static Coordinate2D BrentMinimum(double ax, double bx, ValueAtAbscissa evaluation, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const void * context1 = nullptr, const void * context2 = nullptr, const void * context3 = nullptr);
  static double BrentRoot(double ax, double bx, double precision, ValueAtAbscissa evaluation, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const void * context1 = nullptr, const void * context2 = nullptr, const void * context3 = nullptr);
private:
  constexpr static double k_sqrtEps = 1.4901161193847656E-8; // sqrt(DBL_EPSILON)
  constexpr static double k_goldenRatio = 0.381966011250105151795413165634361882279690820194237137864; // (3-sqrt(5))/2
};

}

#endif
