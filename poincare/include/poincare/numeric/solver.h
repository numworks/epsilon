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
    ReachedDiscontinuity,    // Discontinuity where f(x) is defined
    UnreachedDiscontinuity,  // Discontinuity where f(x) is undefined
    Intersection,
    HorizontalAsymptote,
    YIntercept,
    Other,
  };

  class Solution {
   public:
    Solution(Coordinate2D<T> xy = Coordinate2D<T>(k_NAN, k_NAN),
             Interest interest = Interest::None)
        : m_xy(xy), m_interest(interest) {}
    Solution(T x, T y, Interest interest)
        : Solution(Coordinate2D<T>(x, y), interest) {}

    Coordinate2D<T> xy() const { return m_xy; }
    Interest interest() const { return m_interest; }
    T x() const { return m_xy.x(); }
    T y() const { return m_xy.y(); }

    void setInterest(Interest interest) { m_interest = interest; }
    void setY(T y) { return m_xy.setY(y); }

   private:
    Coordinate2D<T> m_xy;
    Interest m_interest;
  };

  // We need a queue because we can have several solutions at the same abscissa
  class SolutionQueue {
   public:
    SolutionQueue() : m_size(0) {}
    void push(Solution solution) {
      assert(m_size < k_maxSize);
      m_solutions[m_size++] = solution;
    }
    Solution pop() {
      assert(m_size > 0);
      Solution solution = m_solutions[0];
      for (int i = 1; i < m_size; i++) {
        m_solutions[i - 1] = m_solutions[i];
      }
      m_size--;
      return solution;
    }
    bool isEmpty() const { return m_size == 0; }

   private:
    // We can have at most 3 solutions at the same abscissa
    constexpr static int k_maxSize = 3;
    Solution m_solutions[k_maxSize];
    int m_size;
  };

  enum class GrowthSpeed : bool { Fast, Precise };

  typedef T (*FunctionEvaluation)(T, const void*);
  typedef Interest (*BracketTest)(Coordinate2D<T>, Coordinate2D<T>,
                                  Coordinate2D<T>, const void*);
  typedef Coordinate2D<T> (*HoneResult)(FunctionEvaluation, const void*, T, T,
                                        Interest, T, OMG::Troolean);
  typedef bool (*DiscontinuityEvaluation)(Coordinate2D<T>, Coordinate2D<T>,
                                          const void*);

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
  static Interest DiscontinuityInBracket(Coordinate2D<T> a, Coordinate2D<T> b,
                                         Coordinate2D<T> c, const void* aux) {
    return BoolToInterest(DiscontinuityTestBetweenPoints(a, c, aux),
                          Interest::ReachedDiscontinuity);
  }

  /* Arguments beyond xEnd are only required if the Solver manipulates
   * Expression. */
  Solver(T xStart, T xEnd, Context* context = nullptr);

  /* These methods will return the solution in ]xStart,xEnd[ (or ]xEnd,xStart[)
   * closest to xStart, or NAN if it does not exist.
   * TODO_PCJ: The unknown variable must have been projected to id 0. */
  Solution next(const Internal::Tree* e, BracketTest test, HoneResult hone);
  Solution next(FunctionEvaluation f, const void* aux, BracketTest test,
                HoneResult hone,
                DiscontinuityEvaluation discontinuityTest = nullptr);
  Solution nextRoot(const Internal::Tree* e);
  Solution nextRoot(FunctionEvaluation f, const void* aux) {
    return next(f, aux, EvenOrOddRootInBracket, CompositeBrentForRoot);
  }
  Solution nextMinimum(const Internal::Tree* e);
  Solution nextMaximum(const Internal::Tree* e) {
    return next(e, MaximumInBracket, SafeBrentMaximum);
  }
  Solution nextDiscontinuity(const Internal::Tree* e) {
    return next(e, DiscontinuityInBracket, DummyHone);
  }
  /* Caller of nextIntersection may provide a place to store the difference
   * between the two expressions, in case the method needs to be called several
   * times in a row. */
  Solution nextIntersection(
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
  constexpr static T k_NAN = static_cast<T>(NAN);
  constexpr static T k_zero = static_cast<T>(0);
  /* We use k_minimalPracticalStep (10^-6) when stepping around zero instead of
   * k_minimalAbsoluteStep (~10^-8), to avoid wasting time with too many very
   * precise computations. */
  constexpr static T k_minimalPracticalStep =
      std::max(static_cast<T>(1e-6), k_minimalAbsoluteStep);

  static T MagicRound(T x);

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
  static Coordinate2D<T> DummyHone(FunctionEvaluation, const void*, T, T,
                                   Interest, T, OMG::Troolean) {
    assert(false);
    return Coordinate2D<T>();
  }

  static bool DiscontinuityTestBetweenPoints(Coordinate2D<T> a,
                                             Coordinate2D<T> b,
                                             const void* aux);
  static bool DiscontinuityTestAtPoints(Coordinate2D<T> a, Coordinate2D<T> b,
                                        const void* aux);
  static Coordinate2D<T> FindUndefinedIntervalBound(
      Coordinate2D<T> p1, Coordinate2D<T> p2, Coordinate2D<T> p3,
      FunctionEvaluation f, const void* aux, T minimalSizeOfInterval,
      bool findStart);
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
  T nextPossibleRootInChild(const Internal::Tree* e, int childIndex) const;
  typedef bool (*ExpressionTestAuxiliary)(const Internal::Tree* e,
                                          Context* context, void* auxiliary);
  T nextRootInChildren(const Internal::Tree* e, ExpressionTestAuxiliary test,
                       void* aux) const;
  T nextRootInMultiplication(const Internal::Tree* m) const;
  T nextRootInAddition(const Internal::Tree* m) const;
  T nextRootInDependency(const Internal::Tree* m) const;
  void honeAndRoundSolution(FunctionEvaluation f, const void* aux,
                            Coordinate2D<T> start, Coordinate2D<T> end,
                            Interest interest, HoneResult hone,
                            DiscontinuityEvaluation discontinuityTest);
  void honeAndRoundDiscontinuitySolution(FunctionEvaluation f, const void* aux,
                                         Coordinate2D<T> start,
                                         Coordinate2D<T> end);
  bool FindMinimalIntervalContainingDiscontinuity(
      FunctionEvaluation f, const void* aux, Coordinate2D<T>* start,
      Coordinate2D<T>* middle, Coordinate2D<T>* end, T minimalSizeOfInterval);

  Solution registerSolution(Solution solution, bool wasQueued = false);
  Solution registerRoot(T x) {
    return registerSolution(Solution(x, k_zero, Interest::Root));
  }

  T m_xStart;
  T m_xEnd;
  T m_searchStep;
  Context* m_context;
  GrowthSpeed m_growthSpeed;
  SolutionQueue m_solutionQueue;
};

}  // namespace Poincare

#endif
