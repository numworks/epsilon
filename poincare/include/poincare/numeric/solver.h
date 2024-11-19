#ifndef POINCARE_NUMERIC_SOLVER_H
#define POINCARE_NUMERIC_SOLVER_H

#include <omg/float.h>
#include <omg/troolean.h>
#include <poincare/coordinate_2D.h>
#include <poincare/src/expression/context.h>
#include <poincare/src/memory/tree.h>

#include <algorithm>
#include <cmath>

/* TODO_PCJ: Change signatures to systemFunctions instead of Trees. */
namespace Poincare {

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
                                        Interest, T, OMG::Troolean);
  typedef bool (*DiscontinuityEvaluation)(T, T, const void*);

  constexpr static T k_relativePrecision = OMG::Float::Epsilon<T>();
  constexpr static T k_minimalAbsoluteStep =
      2. * OMG::Float::SquareRoot<T>(2. * k_relativePrecision);

  static T NullTolerance(T x) {
    return std::max(
        k_relativePrecision,
        OMG::Float::SquareRoot<T>(k_relativePrecision) * std::fabs(x));
  }
  static T DefaultSearchStepForAmplitude(T intervalAmplitude);

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
  Solver(T xStart, T xEnd, Context* context = nullptr);

  Interest lastInterest() const { return m_lastInterest; }
  Coordinate2D<T> result() const {
    return m_lastInterest == Interest::None
               ? Coordinate2D<T>(k_NAN, k_NAN)
               : Coordinate2D<T>(m_xStart, m_yResult);
  }

  /* These methods will return the solution in ]xStart,xEnd[ (or ]xEnd,xStart[)
   * closest to xStart, or NAN if it does not exist.
   * TODO_PCJ: The unknown variable must have been projected to id 0. */
  Coordinate2D<T> next(const Internal::Tree* e, BracketTest test,
                       HoneResult hone);
  Coordinate2D<T> next(FunctionEvaluation f, const void* aux, BracketTest test,
                       HoneResult hone,
                       DiscontinuityEvaluation discontinuityTest = nullptr);
  Coordinate2D<T> nextRoot(const Internal::Tree* e);
  Coordinate2D<T> nextRoot(FunctionEvaluation f, const void* aux) {
    return next(f, aux, EvenOrOddRootInBracket, CompositeBrentForRoot);
  }
  Coordinate2D<T> nextMinimum(const Internal::Tree* e);
  Coordinate2D<T> nextMaximum(const Internal::Tree* e) {
    return next(e, MaximumInBracket, SafeBrentMaximum);
  }
  /* Caller of nextIntersection may provide a place to store the difference
   * between the two expressions, in case the method needs to be called several
   * times in a row. */
  Coordinate2D<T> nextIntersection(
      const Internal::Tree* e1, const Internal::Tree* e2,
      const Internal::Tree** memoizedDifference = nullptr);
  /* Stretch the interval to include the previous bounds. This allows finding
   * solutions in [xStart,xEnd], as otherwise all resolution is done on an open
   * interval. */
  void stretch();
  void setSearchStep(T step) {
    m_searchStep = std::max(step, k_minimalPracticalStep);
  }
  void setGrowthSpeed(GrowthSpeed speed) { m_growthSpeed = speed; }

 private:
  struct FunctionEvaluationParameters {
    // const ApproximationContext &approximationContext;
    const Internal::Tree* expression;
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
                                          T xPrecision,
                                          OMG::Troolean discontinuous);
  static Coordinate2D<T> SafeBrentMaximum(FunctionEvaluation f, const void* aux,
                                          T xMin, T xMax, Interest interest,
                                          T xPrecision,
                                          OMG::Troolean discontinuous);
  static Coordinate2D<T> CompositeBrentForRoot(FunctionEvaluation f,
                                               const void* aux, T xMin, T xMax,
                                               Interest interest, T xPrecision,
                                               OMG::Troolean discontinuous);

  static bool DiscontinuityTestForExpression(T x1, T x2, const void* aux);
  static Coordinate2D<T> FindUndefinedIntervalExtremum(
      Coordinate2D<T> p1, Coordinate2D<T> p2, Coordinate2D<T> p3,
      FunctionEvaluation f, const void* aux, T minimalSizeOfInterval,
      bool findStart);
  static Coordinate2D<T> FindUndefinedIntervalStart(
      Coordinate2D<T> p1, Coordinate2D<T> p2, Coordinate2D<T> p3,
      FunctionEvaluation f, const void* aux, T minimalSizeOfInterval) {
    return FindUndefinedIntervalExtremum(p1, p2, p3, f, aux,
                                         minimalSizeOfInterval, true);
  }
  static Coordinate2D<T> FindUndefinedIntervalEnd(
      Coordinate2D<T> p1, Coordinate2D<T> p2, Coordinate2D<T> p3,
      FunctionEvaluation f, const void* aux, T minimalSizeOfInterval) {
    return FindUndefinedIntervalExtremum(p1, p2, p3, f, aux,
                                         minimalSizeOfInterval, false);
  }
  static void ExcludeUndefinedFromBracket(Coordinate2D<T>* p1,
                                          Coordinate2D<T>* p2,
                                          Coordinate2D<T>* p3,
                                          FunctionEvaluation f, const void* aux,
                                          T minimalSizeOfInterval);
  static bool FunctionSeemsConstantOnTheInterval(
      Solver<T>::FunctionEvaluation f, const void* aux, T xMin, T xMax);

  static T MinimalStep(T x, T slope = static_cast<T>(1.));
  bool validSolution(T x) const;
  T nextX(T x, T direction, T slope) const;
  Coordinate2D<T> nextPossibleRootInChild(const Internal::Tree* e,
                                          int childIndex) const;
  typedef bool (*ExpressionTestAuxiliary)(const Internal::Tree* e,
                                          Context* context, void* auxiliary);
  Coordinate2D<T> nextRootInChildren(const Internal::Tree* e,
                                     ExpressionTestAuxiliary test,
                                     void* aux) const;
  Coordinate2D<T> nextRootInMultiplication(const Internal::Tree* m) const;
  Coordinate2D<T> nextRootInAddition(const Internal::Tree* m) const;
  Coordinate2D<T> nextRootInDependency(const Internal::Tree* m) const;
  Coordinate2D<T> honeAndRoundSolution(
      FunctionEvaluation f, const void* aux, T start, T end, Interest interest,
      HoneResult hone, DiscontinuityEvaluation discontinuityTest);
  void registerSolution(Coordinate2D<T> solution, Interest interest);

  T m_xStart;
  T m_xEnd;
  T m_searchStep;
  T m_yResult;
  Context* m_context;
  Interest m_lastInterest;
  GrowthSpeed m_growthSpeed;
};

}  // namespace Poincare

#endif
