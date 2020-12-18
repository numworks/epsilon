#ifndef POINCARE_SOLVER_H
#define POINCARE_SOLVER_H

#include <poincare/context.h>
#include <poincare/coordinate_2D.h>
#include <poincare/preferences.h>

namespace Poincare {

template<typename T>
class SolverHelper {
public:
  typedef T (*ValueAtAbscissa)(T abscissa, Context * context, const void * auxiliary);
  typedef Coordinate2D<T> (*BracketSearch)(T a, T b, T c, T fa, T fb, T fc, ValueAtAbscissa f, Context * context, const void * auxiliary);

  static bool RootExistsOnInterval(T fa, T fb, T fc);
  static bool MinimumExistsOnInterval(T fa, T fb, T fc) { return (std::isnan(fa) || fa > fb) && (std::isnan(fc) || fb < fc) && (!std::isnan(fa) || !std::isnan(fc)); }
  static bool MaximumExistsOnInterval(T fa, T fb, T fc) { return (std::isnan(fa) || fa < fb) && (std::isnan(fc) || fb > fc) && (!std::isnan(fa) || !std::isnan(fc)); }
};

class Solver {
public:
  // Minimum
  typedef double (*ValueAtAbscissa)(double abscissa, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const void * context1, const void * context2, const void * context3);
  static Coordinate2D<double> BrentMinimum(double ax, double bx, ValueAtAbscissa evaluation, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const void * context1 = nullptr, const void * context2 = nullptr, const void * context3 = nullptr);

  // Root
  static double BrentRoot(double ax, double bx, double precision, ValueAtAbscissa evaluation, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const void * context1 = nullptr, const void * context2 = nullptr, const void * context3 = nullptr);
  static Coordinate2D<double> IncreasingFunctionRoot(double ax, double bx, double resultPrecision, ValueAtAbscissa evaluation, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const void * context1 = nullptr, const void * context2 = nullptr, const void * context3 = nullptr, double * resultEvaluation = nullptr);

  // Proba

  // Cumulative distributive inverse for function defined on N (positive integers)
  template<typename T> static T CumulativeDistributiveInverseForNDefinedFunction(T * probability, ValueAtAbscissa evaluation, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const void * context1 = nullptr, const void * context2 = nullptr, const void * context3 = nullptr);

  // Cumulative distributive function for function defined on N (positive integers)
  template<typename T> static T CumulativeDistributiveFunctionForNDefinedFunction(T x, ValueAtAbscissa evaluation, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, const void * context1 = nullptr, const void * context2 = nullptr, const void * context3 = nullptr);

private:
  constexpr static int k_maxNumberOfOperations = 1000000;
  constexpr static double k_maxProbability = 0.9999995;
  constexpr static double k_sqrtEps = 1.4901161193847656E-8; // sqrt(DBL_EPSILON)
  constexpr static double k_goldenRatio = 0.381966011250105151795413165634361882279690820194237137864; // (3-sqrt(5))/2
};

}

#endif
