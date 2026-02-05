#pragma once

#include <omg/float.h>
#include <omg/troolean.h>
#include <omg/vector.h>
#include <poincare/coordinate_2D.h>
#include <poincare/symbol_context.h>
#include <poincare/tree.h>

#include <algorithm>
#include <cmath>

#include "interest.h"

/* TODO_PCJ: Change signatures to preparedFunctions instead of Trees. */
namespace Poincare {

template <typename T>
class Solver {
 public:
  struct Solution {
    T x;
    T y;
    Interest interest;

    constexpr Solution(T xValue, T yValue, Interest interestValue)
        : x(xValue), y(yValue), interest(interestValue) {}
    constexpr Solution(Coordinate2D<T> p, Interest interestValue)
        : x(p.x()), y(p.y()), interest(interestValue) {}
    constexpr Solution() : x(k_NAN), y(k_NAN), interest(Interest::None) {}
    constexpr operator Coordinate2D<T>() const { return Coordinate2D<T>(x, y); }
  };

  // We need at most 3 solutions at the same abscissa for discontinuities
  constexpr static int k_maxSolutionsAtSameAbscissa = 3;
  // We need a queue because we can have several solutions at the same abscissa
  class SolutionQueue
      : public OMG::StaticVector<Solution, k_maxSolutionsAtSameAbscissa> {
   public:
    Solution popFirst() {
      assert(this->size() > 0);
      Solution solution = (*this)[0];
      this->removeAt(0);
      return solution;
    }
  };

  enum class GrowthSpeed : bool { Fast, Precise };

  typedef T (*FunctionEvaluation)(T, const void*);
  typedef Interest (*BracketTest)(Coordinate2D<T>, Coordinate2D<T>,
                                  Coordinate2D<T>, const void*);
  typedef Coordinate2D<T> (*HoneResult)(FunctionEvaluation, const void*, T, T,
                                        Interest, T);
  typedef bool (*DiscontinuityEvaluation)(Coordinate2D<T>, Coordinate2D<T>,
                                          const void*);

  constexpr static T k_relativePrecision = OMG::Float::Epsilon<T>();
  constexpr static T k_sqrtRelativePrecision = OMG::Float::SqrtEpsilon<T>();
  constexpr static T k_minimalAbsoluteStep =
      2. * OMG::Float::SquareRoot<T>(2. * k_relativePrecision);

  static T NullTolerance(T x) {
    return std::max(k_relativePrecision,
                    k_sqrtRelativePrecision * std::fabs(x));
  }
  static T DefaultSearchStepForAmplitude(T intervalAmplitude);

  // If areAlongSameAxis, f-g is computed. Else, a->g(f(a))-a is computed
  static Internal::Tree* FunctionDifferenceForIntersection(
      const Internal::Tree* f, const Internal::Tree* g, bool areAlongSameAxis);

  /* Arguments beyond xEnd are only required if the Solver manipulates
   * Expression. */
  Solver(T xStart, T xEnd,
         const SymbolContext& symbolContext = EmptySymbolContext{});

  void reset(T xStart, T xEnd, T searchStep);

  /* These methods will return the solution in ]xStart,xEnd[ (or ]xEnd,xStart[)
   * closest to xStart, or NAN if it does not exist.
   * TODO_PCJ: The unknown variable must have been projected to id 0. */
  Solution next(const Internal::Tree* e, BracketTest test, HoneResult hone);
  Solution next(FunctionEvaluation f, const void* aux, BracketTest test,
                HoneResult hone,
                DiscontinuityEvaluation discontinuityTest = nullptr);
  Solution nextRoot(const Internal::Tree* e) {
    return nextDetailedRoot(e).solution;
  }
  Solution nextRoot(FunctionEvaluation f, const void* aux);
  Solution nextMinimum(const Internal::Tree* e);
  Solution nextMaximum(const Internal::Tree* e);
  Solution nextDiscontinuity(const Internal::Tree* e);

  /* Difference between the two functions may be passed to save computations
   * (see FunctionDifferenceForIntersection), when the function is called
   * several times */
  Solution nextIntersection(const Internal::Tree* e1, const Internal::Tree* e2,
                            bool areAlongSameAxis,
                            const Internal::Tree* difference);
  Solution nextIntersection(const Internal::Tree* e1, const Internal::Tree* e2,
                            bool areAlongSameAxis = true);

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
                                          T xPrecision);
  static Coordinate2D<T> SafeBrentMaximum(FunctionEvaluation f, const void* aux,
                                          T xMin, T xMax, Interest interest,
                                          T xPrecision);
  static Coordinate2D<T> CompositeBrentForRoot(FunctionEvaluation f,
                                               const void* aux, T xMin, T xMax,
                                               Interest interest, T xPrecision);
  static Coordinate2D<T> DummyHone(FunctionEvaluation, const void*, T, T,
                                   Interest, T) {
    assert(false);
    return Coordinate2D<T>();
  }

  static bool FunctionSeemsConstantOnTheInterval(
      Solver<T>::FunctionEvaluation f, const void* aux, T xMin, T xMax);

  static T MinimalStep(T x, T slope = static_cast<T>(1.));
  bool validSolution(T x) const;
  T nextX(T x, T direction, T slope) const;

  /* Stores the root and a pointer to the minimal sub-expression
   * that evaluates to zero (mainly used for dependencies handling) */
  struct DetailedRoot {
    Solution solution;
    const Internal::Tree* nullDescendant = nullptr;
  };
  struct DetailedRootX {
    T x;
    const Internal::Tree* nullDescendant = nullptr;
  };
  constexpr static DetailedRootX k_nanRootX = {.x = k_NAN,
                                               .nullDescendant = nullptr};

  DetailedRoot nextDetailedRoot(const Internal::Tree* e);
  // Find the next root of the child which is also a root of the parent.
  DetailedRootX nextRootSearchingInChild(const Internal::Tree* e,
                                         int childIndex) const;
  typedef bool (*ExpressionTestAuxiliary)(const Internal::Tree* e,
                                          const SymbolContext& symbolContext,
                                          void* auxiliary);
  /* Find the next root among the children satisfying the test, which is also a
   * root of the parent. */
  DetailedRootX nextRootSearchingInChildren(const Internal::Tree* e,
                                            ExpressionTestAuxiliary test,
                                            void* aux) const;
  DetailedRootX nextRootInMultiplication(const Internal::Tree* m) const;
  DetailedRootX nextRootInAddition(const Internal::Tree* m) const;
  DetailedRootX nextRootInDependency(const Internal::Tree* m) const;
  void honeAndRoundSolution(FunctionEvaluation f, const void* aux,
                            Coordinate2D<T> start, Coordinate2D<T> end,
                            Interest interest, HoneResult hone,
                            DiscontinuityEvaluation discontinuityTest);
  void honeAndRoundDiscontinuitySolution(FunctionEvaluation f, const void* aux,
                                         Coordinate2D<T> start,
                                         Coordinate2D<T> end);
  bool FindMinimalIntervalContainingDiscontinuity(
      FunctionEvaluation f, const void* aux, Coordinate2D<T>* start,
      Coordinate2D<T>* middle, Coordinate2D<T>* end, T minimalSizeOfInterval,
      DiscontinuityEvaluation discontinuityTest);

  Solution registerSolution(Solution solution, bool wasQueued = false);
  DetailedRoot registerDetailedRoot(DetailedRootX rootAbscissa) {
    return {.solution = registerSolution(
                Solution(rootAbscissa.x, k_zero, Interest::Root)),
            .nullDescendant = rootAbscissa.nullDescendant};
  }

  Interest TestBetween(Coordinate2D<T> a, Coordinate2D<T> b, BracketTest test,
                       FunctionEvaluation f, const void* aux);

  T m_xStart;
  T m_xEnd;
  T m_searchStep;
  const SymbolContext& m_context;
  GrowthSpeed m_growthSpeed;
  SolutionQueue m_solutionQueue;
};

}  // namespace Poincare
