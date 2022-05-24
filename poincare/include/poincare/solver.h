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

  static Coordinate2D<T> NextPointOfInterest(
      ValueAtAbscissa evaluation, Context * context, const void * auxiliary,
      BracketSearch search,
      T start, T end,
      T relativePrecision,
      T minimalStep, T maximalStep);

  static bool RootExistsOnInterval(T fa, T fb, T fc);
  static bool MinimumExistsOnInterval(T fa, T fb, T fc) { return (std::isnan(fa) || fa > fb) && (std::isnan(fc) || fb < fc) && (!std::isnan(fa) || !std::isnan(fc)); }
  static bool MaximumExistsOnInterval(T fa, T fb, T fc) { return MinimumExistsOnInterval(-fa, -fb, -fc); }

private:
  static Coordinate2D<T> NextPointOfInterestHelper(ValueAtAbscissa evaluation, Context * context, const void * auxiliary, BracketSearch search, T start, T end, T relativePrecision, T minimalStep, T maximalStep);
  static T Step(T x, T growthSpeed, T minimalStep, T maximalStep);
};

class Solver {
public:
  static constexpr double k_zeroPrecision = 1e-5;
  static constexpr double k_relativePrecision = 1e-2;
  static constexpr double k_minimalStep = 1e-3;

  typedef SolverHelper<double>::ValueAtAbscissa ValueAtAbscissa;

  // Minimum
  static Coordinate2D<double> NextMinimum(ValueAtAbscissa evaluation, Context * context, const void * auxiliary, double start, double end, double relativePrecision, double minimalStep, double maximalStep);

  // Root
  static double NextRoot(ValueAtAbscissa evaluation, Context * context, const void * auxiliary, double start, double end, double relativePrecision, double minimalStep, double maximalStep);
  static Coordinate2D<double> IncreasingFunctionRoot(double ax, double bx, double resultPrecision, ValueAtAbscissa evaluation, Context * context, const void * auxiliary, double * resultEvaluation = nullptr);

  // Probabilities
  // Cumulative distributive inverse for function defined on N (positive integers)
  template<typename T> static T CumulativeDistributiveInverseForNDefinedFunction(T * probability, typename SolverHelper<T>::ValueAtAbscissa evaluation, Context * context, const void * auxiliary);
  // Cumulative distributive function for function defined on N (positive integers)
  template<typename T> static T CumulativeDistributiveFunctionForNDefinedFunction(T x, typename SolverHelper<T>::ValueAtAbscissa evaluation, Context * context, const void * auxiliary);

  static double DefaultMaximalStep(double start, double stop);

private:
  constexpr static int k_maxNumberOfOperations = 1000000;
  constexpr static double k_maxProbability = 0.9999995;
  constexpr static double k_sqrtEps = 1.4901161193847656E-8; // sqrt(DBL_EPSILON)
  constexpr static double k_goldenRatio = 0.381966011250105151795413165634361882279690820194237137864; // (3-sqrt(5))/2
  static constexpr double k_maxFloat = 1e100;
  static constexpr double k_precisionByGradUnit = 1e6;

  static Coordinate2D<double> BrentMinimum(double ax, double bx, ValueAtAbscissa evaluation, Context * context, const void * auxiliary);
  static double BrentRoot(double ax, double bx, double precision, ValueAtAbscissa evaluation, Context * context, const void * auxiliary);
  static Coordinate2D<double> RoundCoordinatesToZero(Coordinate2D<double> xy, double a, double b, ValueAtAbscissa f, Context * context, const void * auxiliary);
};

}

#endif
