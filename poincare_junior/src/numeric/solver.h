#ifndef POINCARE_JUNIOR_NUMERIC_SOLVER_H
#define POINCARE_JUNIOR_NUMERIC_SOLVER_H

#include <math.h>
#include <omgpj/troolean.h>
#include <poincare_junior/src/expression/context.h>
#include <poincare_junior/src/memory/tree.h>

#include <algorithm>

#include "coordinate_2D.h"
#include "float.h"

namespace PoincareJ {

class Context;

template <typename T>
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
    HorizontalAsymptote,
    YIntercept,
    Other,
  };

  enum class GrowthSpeed : bool { Fast, Precise };

  typedef T (*FunctionEvaluation)(T, const void*);
  typedef Interest (*BracketTest)(Coordinate2D<T>, Coordinate2D<T>,
                                  Coordinate2D<T>, const void*);
  typedef Coordinate2D<T> (*HoneResult)(FunctionEvaluation, const void*, T, T,
                                        Interest, T, Troolean);
  typedef bool (*DiscontinuityEvaluation)(T, T, const void*);

  constexpr static T k_relativePrecision = Float<T>::Epsilon();
  constexpr static T k_minimalAbsoluteStep =
      2. * Float<T>::SquareRoot(2. * k_relativePrecision);

  static T NullTolerance(T x) {
    return std::max(k_relativePrecision,
                    Float<T>::SquareRoot(k_relativePrecision) * std::fabs(x));
  }
  static T MaximalStep(T intervalAmplitude);

  // BracketTest default implementations
  constexpr static Interest BoolToInterest(bool v, Interest t,
                                           Interest f = Interest::None) {
    return v ? t : f;
  }
  static Interest OddRootInBracket(Coordinate2D<T> a, Coordinate2D<T> b,
                                   Coordinate2D<T> c, const void*) {
    return BoolToInterest(
        (a.y() < 0. && 0. < c.y()) || (c.y() < 0. && 0. < a.y()),
        Interest::Root);
  }
  static Interest EvenOrOddRootInBracket(Coordinate2D<T> a, Coordinate2D<T> b,
                                         Coordinate2D<T> c, const void*);
  static Interest MinimumInBracket(Coordinate2D<T> a, Coordinate2D<T> b,
                                   Coordinate2D<T> c, const void*) {
    return BoolToInterest(b.y() < a.y() && b.y() < c.y(),
                          Interest::LocalMinimum);
  }
  static Interest MaximumInBracket(Coordinate2D<T> a, Coordinate2D<T> b,
                                   Coordinate2D<T> c, const void*) {
    return BoolToInterest(a.y() < b.y() && c.y() < b.y(),
                          Interest::LocalMaximum);
  }
  static Interest UndefinedInBracket(Coordinate2D<T> a, Coordinate2D<T> b,
                                     Coordinate2D<T> c, const void*) {
    return BoolToInterest((std::isfinite(a.y()) && std::isnan(c.y())) ||
                              (std::isfinite(c.y()) && std::isnan(a.y())),
                          Interest::Discontinuity);
  }

  /* Arguments beyond xEnd are only required if the Solver manipulates
   * Expression. */
  Solver(T xStart, T xEnd, const char* unknown = nullptr,
         Context* context = nullptr,
         ComplexFormat complexFormat = ComplexFormat::Cartesian,
         AngleUnit angleUnit = AngleUnit::Radian);

  T start() const { return m_xStart; }
  T end() const { return m_xEnd; }
  Interest lastInterest() const { return m_lastInterest; }
  Coordinate2D<T> result() const {
    return lastInterest() == Interest::None
               ? Coordinate2D<T>(k_NAN, k_NAN)
               : Coordinate2D<T>(start(), m_yResult);
  }

  /* These methods will return the solution in ]xStart,xEnd[ (or ]xEnd,xStart[)
   * closest to xStart, or NAN if it does not exist.
   * TODO_PCJ: The unknown variable must have been projected to id 0. */
  Coordinate2D<T> next(const Tree* e, BracketTest test, HoneResult hone);
  Coordinate2D<T> next(FunctionEvaluation f, const void* aux, BracketTest test,
                       HoneResult hone,
                       DiscontinuityEvaluation discontinuityTest = nullptr);
  Coordinate2D<T> nextRoot(const Tree* e);
  Coordinate2D<T> nextRoot(FunctionEvaluation f, const void* aux) {
    return next(f, aux, EvenOrOddRootInBracket, CompositeBrentForRoot);
  }
  Coordinate2D<T> nextMinimum(const Tree* e);
  Coordinate2D<T> nextMaximum(const Tree* e) {
    return next(e, MaximumInBracket, SafeBrentMaximum);
  }
  /* Caller of nextIntersection may provide a place to store the difference
   * between the two expressions, in case the method needs to be called several
   * times in a row. */
  Coordinate2D<T> nextIntersection(const Tree* e1, const Tree* e2,
                                   const Tree** memoizedDifference = nullptr);
  /* Stretch the interval to include the previous bounds. This allows finding
   * solutions in [xStart,xEnd], as otherwise all resolution is done on an open
   * interval. */
  void stretch();
  void setSearchStep(T step) { m_maximalXStep = step; }
  void setGrowthSpeed(GrowthSpeed speed) { m_growthSpeed = speed; }

 private:
  struct FunctionEvaluationParameters {
    // const ApproximationContext &approximationContext;
    const char* unknown;
    const Tree* expression;
  };

  constexpr static T k_NAN = static_cast<T>(NAN);
  constexpr static T k_zero = static_cast<T>(0);
  /* We use k_minimalPracticalStep (10^-6) when stepping around zero instead of
   * k_minimalAbsoluteStep (~10^-8), to avoid wasting time with too many very
   * precise computations. */
  constexpr static T k_minimalPracticalStep =
      std::max(static_cast<T>(1e-6), k_minimalAbsoluteStep);

  static Coordinate2D<T> SafeBrentMinimum(FunctionEvaluation f, const void* aux,
                                          T xMin, T xMax, Interest interest,
                                          T precision, Troolean discontinuous);
  static Coordinate2D<T> SafeBrentMaximum(FunctionEvaluation f, const void* aux,
                                          T xMin, T xMax, Interest interest,
                                          T precision, Troolean discontinuous);
  static Coordinate2D<T> CompositeBrentForRoot(FunctionEvaluation f,
                                               const void* aux, T xMin, T xMax,
                                               Interest interest, T precision,
                                               Troolean discontinuous);

  static bool DiscontinuityTestForExpression(T x1, T x2, const void* aux);
  static void ExcludeUndefinedFromBracket(Coordinate2D<T>* p1,
                                          Coordinate2D<T>* p2,
                                          Coordinate2D<T>* p3,
                                          FunctionEvaluation f, const void* aux,
                                          T minimalSizeOfInterval);
  static bool FunctionSeemsConstantOnTheInterval(
      Solver<T>::FunctionEvaluation f, const void* aux, T xMin, T xMax);

  T maximalStep() const { return m_maximalXStep; }
  static T MinimalStep(T x, T slope = static_cast<T>(1.));
  bool validSolution(T x) const;
  T nextX(T x, T direction, T slope) const;
  Coordinate2D<T> nextPossibleRootInChild(const Tree* e, int childIndex) const;
  typedef bool (*ExpressionTestAuxiliary)(const Tree* e, Context* context,
                                          void* auxiliary);
  Coordinate2D<T> nextRootInChildren(const Tree* e,
                                     ExpressionTestAuxiliary test,
                                     void* aux) const;
  Coordinate2D<T> nextRootInMultiplication(const Tree* m) const;
  Coordinate2D<T> nextRootInAddition(const Tree* m) const;
  Coordinate2D<T> honeAndRoundSolution(
      FunctionEvaluation f, const void* aux, T start, T end, Interest interest,
      HoneResult hone, DiscontinuityEvaluation discontinuityTest);
  void registerSolution(Coordinate2D<T> solution, Interest interest);

  T m_xStart;
  T m_xEnd;
  T m_maximalXStep;
  T m_yResult;
  Context* m_context;
  const char* m_unknown;
  ComplexFormat m_complexFormat;
  AngleUnit m_angleUnit;
  Interest m_lastInterest;
  GrowthSpeed m_growthSpeed;
};

}  // namespace PoincareJ

#endif
