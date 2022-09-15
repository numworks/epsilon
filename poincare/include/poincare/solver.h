#ifndef POINCARE_SOLVER_H
#define POINCARE_SOLVER_H

#include <poincare/expression.h>
#include <poincare/float.h>
#include <math.h>

namespace Poincare {

template<typename T>
class Solver {
public:
  enum class Interest : uint8_t {
    None,
    Root,
    LocalMinimum,
    LocalMaximum,
    GlobalMinimum,
    GlobalMaximum,
    Discontinuity,
    Intersection,
    Other,
  };

  typedef T (*FunctionEvaluation)(T, const void *);
  typedef Interest (*BracketTest)(T, T, T);
  typedef Coordinate2D<T> (*HoneResult)(FunctionEvaluation, const void *, T, T, Interest, T);

  /* Arguments beyond xEnd are only required if the Solver manipulates
   * Expression. */
  Solver(T xStart, T xEnd, const char * unknown = nullptr, Context * context = nullptr, Preferences::ComplexFormat complexFormat = Preferences::ComplexFormat::Cartesian, Preferences::AngleUnit angleUnit = Preferences::AngleUnit::Radian, T precision = Float<T>::Epsilon());
  Solver(const Solver<T> * other);

  T start() const { return m_xStart; }
  T end() const { return m_xEnd; }
  Interest lastInterest() const { return m_lastInterest; }
  Coordinate2D<T> result() const { return lastInterest() == Interest::None ? Coordinate2D<T>(k_NAN, k_NAN) : Coordinate2D<T>(start(), m_yResult); }
  /* These methods will return the solution in ]xStart,xEnd[ (or ]xEnd,xStart[)
   * closest to xStart, or NAN if it does not exist. */
  Coordinate2D<T> next(Expression e, BracketTest test, HoneResult hone);
  Coordinate2D<T> next(FunctionEvaluation f, const void * aux, BracketTest test, HoneResult hone);
  Coordinate2D<T> nextRoot(Expression e);
  Coordinate2D<T> nextRoot(FunctionEvaluation f, const void * aux) { return next(f, aux, EvenOrOddRootInBracket, CompositeBrentForRoot); }
  Coordinate2D<T> nextMinimum(Expression e);
  Coordinate2D<T> nextMaximum(Expression e) { return next(e, MaximumInBracket, BrentMaximum); }
  Coordinate2D<T> nextIntersection(Expression e1, Expression e2);
  /* Stretch the interval to include the previous bounds. This allows finding
   * solutions in [xStart,xEnd], as otherwise all resolution is done on an open
   * interval. */
  void stretch();

private:
  struct FunctionEvaluationParameters {
    Context * context;
    const char * unknown;
    Expression expression;
    Preferences::ComplexFormat complexFormat;
    Preferences::AngleUnit angleUnit;
  };

  constexpr static T k_NAN = static_cast<T>(NAN);
  constexpr static T k_zero = static_cast<T>(0.);
  constexpr static T k_minimalAbsoluteStep = static_cast<T>(1e-3);

  static T NullTolerance(T precision) { /* TODO */ return precision; }
  constexpr static Interest BoolToInterest(bool v, Interest t, Interest f = Interest::None) { return v ? t : f; }
  // Call SolverAlgorithms::BrentMinimum on the opposite evaluation
  static Coordinate2D<T> BrentMaximum(FunctionEvaluation f, const void * aux, T xMin, T xMax, Interest interest, T precision);
  static Coordinate2D<T> CompositeBrentForRoot(FunctionEvaluation f, const void * aux, T xMin, T xMax, Interest interest, T precision);
  // BracketTest default implementations
  static Interest OddRootInBracket(T a, T b, T c) { return BoolToInterest((a < k_zero && k_zero < c) || (c < k_zero && k_zero < a), Interest::Root); }
  static Interest EvenOrOddRootInBracket(T a, T b, T c);
  static Interest MinimumInBracket(T a, T b, T c) { return BoolToInterest(b < a && b < c, Interest::LocalMinimum); }
  static Interest MaximumInBracket(T a, T b, T c) { return BoolToInterest(a < b && c < b, Interest::LocalMaximum); }

  T maximalStep() const;
  T nextX(T x, T direction) const;
  Coordinate2D<T> nextPossibleRootInChild(Expression e, int childIndex) const;
  Coordinate2D<T> nextRootInMultiplication(Expression m) const;
  void registerSolution(Coordinate2D<T> solution, Interest interest);

  T m_xStart;
  T m_xEnd;
  T m_yResult;
  T m_precision;
  Context * m_context;
  const char * m_unknown;
  Preferences::ComplexFormat m_complexFormat;
  Preferences::AngleUnit m_angleUnit;
  Interest m_lastInterest;
};

#if 1
/* FIXME Temporarily keep SolverHelper until Zoom is refactored to use Solver. */

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
#endif

}

#endif
