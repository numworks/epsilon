#pragma once

#include <omg/float.h>
#include <omg/vector.h>
#include <poincare/numeric_solver/interest.h>

#include <algorithm>
#include <cmath>

#include "solution.h"

namespace Poincare::NumericSolver {

template <typename T>
class Solver {
 public:
  typedef T (*FunctionEvaluation)(T, const void*);
  typedef Interest (*BracketTest)(Coordinate2D<T>, Coordinate2D<T>,
                                  Coordinate2D<T>, const void*);
  typedef Coordinate2D<T> (*HoneResult)(FunctionEvaluation, const void*, T, T,
                                        Interest, T);
  typedef bool (*DiscontinuityEvaluation)(Coordinate2D<T>, Coordinate2D<T>,
                                          const void*);

  Solver(T xStart, T xEnd, FunctionEvaluation f, const void* aux,
         BracketTest bracketTest, HoneResult honeFunction,
         DiscontinuityEvaluation discontinuityTest);

  enum class GrowthSpeed : bool { Fast, Precise };

  constexpr static T k_relativePrecision = OMG::Float::Epsilon<T>();
  constexpr static T k_sqrtRelativePrecision = OMG::Float::SqrtEpsilon<T>();
  constexpr static T k_minimalAbsoluteStep =
      2. * OMG::Float::SquareRoot<T>(2. * k_relativePrecision);

  static T NullTolerance(T x) {
    return std::max(k_relativePrecision,
                    k_sqrtRelativePrecision * std::fabs(x));
  }
  static T DefaultSearchStepForAmplitude(T intervalAmplitude);

  void reset(T xStart, T xEnd, T searchStep);

  Solution<T> next();

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

  T evaluateAt(T x) const { return m_evaluator(x, m_auxiliary); }
  // Returns false if no discontinuity test is defined
  bool isDiscontinuousBetween(Coordinate2D<T> a, Coordinate2D<T> b) const {
    return m_discontinuityTest ? m_discontinuityTest(a, b, m_auxiliary) : false;
  }

  static T MagicRound(T x);

  bool seemsConstantOnTheInterval(T xMin, T xMax) const;

  static T MinimalStep(T x, T slope = static_cast<T>(1.));
  bool validSolution(T x) const;
  T nextX(T x, T direction, T slope) const;

  void honeAndRoundSolution(Coordinate2D<T> start, Coordinate2D<T> end,
                            Interest interest);
  void honeAndRoundDiscontinuitySolution(Coordinate2D<T> start,
                                         Coordinate2D<T> end);
  bool findMinimalIntervalContainingDiscontinuity(
      Coordinate2D<T>* start, Coordinate2D<T>* middle, Coordinate2D<T>* end,
      T minimalSizeOfInterval) const;

  Solution<T> registerSolution(Solution<T> solution, bool wasQueued = false);

  Interest testBetween(Coordinate2D<T> a, Coordinate2D<T> b);

  // We need at most 3 solutions at the same abscissa for discontinuities
  constexpr static int k_maxSolutionsAtSameAbscissa = 3;
  // We need a queue because we can have several solutions at the same abscissa
  class SolutionQueue
      : public OMG::StaticVector<Solution<T>, k_maxSolutionsAtSameAbscissa> {
   public:
    Solution<T> popFirst() {
      assert(this->size() > 0);
      Solution<T> solution = (*this)[0];
      this->removeAt(0);
      return solution;
    }
  };

  T m_xStart;
  T m_xEnd;
  T m_searchStep;
  GrowthSpeed m_growthSpeed;
  SolutionQueue m_solutionQueue;
  FunctionEvaluation m_evaluator;
  BracketTest m_bracketTest;
  HoneResult m_honeFunction;
  DiscontinuityEvaluation m_discontinuityTest;
  const void* m_auxiliary;
};

}  // namespace Poincare::NumericSolver
