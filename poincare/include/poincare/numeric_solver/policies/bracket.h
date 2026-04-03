#pragma once

#include <poincare/src/expression/continuity.h>
#include <poincare/src/numeric_solver/hone_functions.h>
#include <poincare/src/numeric_solver/solver_tests.h>

#include "discontinuity.h"

namespace Poincare::NumericSolver {

class RootBracketer {
 public:
  Interest test(Coordinate2D<double> a, Coordinate2D<double> b,
                Coordinate2D<double> c, const void*) {
    return Internal::SolverTests<double>::EvenOrOddRootInBracket(a, b, c,
                                                                 nullptr);
  }
  template <typename TEvaluator>
  Coordinate2D<double> hone(double xMin, double xMax, Interest interest,
                            double precision, const TEvaluator& evaluator) {
    return Internal::HoneFunctions::CompositeBrentForRoot(evaluator, xMin, xMax,
                                                          interest, precision);
  }
};

class ExtremumBracketer {
 public:
  Interest test(Coordinate2D<double> a, Coordinate2D<double> b,
                Coordinate2D<double> c, const void*) {
    return Internal::SolverTests<double>::ExtremumInBracket(a, b, c, nullptr);
  }
  template <typename TEvaluator>
  Coordinate2D<double> hone(double xMin, double xMax, Interest interest,
                            double precision, const TEvaluator& evaluator) {
    return Internal::HoneFunctions::SafeBrentExtremum(evaluator, xMin, xMax,
                                                      interest, precision);
  }
};

class DiscontinuityBracketer {
 public:
  Interest test(Coordinate2D<double> a, Coordinate2D<double> b,
                Coordinate2D<double> c, const Internal::Tree* eval) {
    return ExpressionDiscontinuityTester<double>::IsDiscontinuousBetween(a, c,
                                                                         eval)
               ? Interest::ReachedDiscontinuity
               : Interest::None;
  }
  // Hone is never reached: DiscontinuitySolver overrides findAndPushSolution.
  template <typename TEvaluator>
  Coordinate2D<double> hone(double, double, Interest, double,
                            const TEvaluator&) {
    OMG_UNREACHABLE;
  }
};

template <typename TFloat>
using BracketTest = Interest (*)(Coordinate2D<TFloat>, Coordinate2D<TFloat>,
                                 Coordinate2D<TFloat>, const void*);

template <typename TFloat>
using HoneResult = Coordinate2D<TFloat> (*)(const LambdaEvaluator<TFloat>&,
                                            TFloat, TFloat, Interest, TFloat);

template <typename TFloat>
class LambdaBracketer {
 public:
  LambdaBracketer(BracketTest<TFloat> test, HoneResult<TFloat> hone)
      : m_test(test), m_hone(hone) {}

  Interest test(Coordinate2D<TFloat> a, Coordinate2D<TFloat> b,
                Coordinate2D<TFloat> c, const void* aux) const {
    return m_test(a, b, c, aux);
  }

  Coordinate2D<TFloat> hone(TFloat xMin, TFloat xMax, Interest interest,
                            TFloat precision,
                            const LambdaEvaluator<TFloat>& evaluator) const {
    return m_hone(evaluator, xMin, xMax, interest, precision);
  }

 private:
  BracketTest<TFloat> m_test;
  HoneResult<TFloat> m_hone;
};

}  // namespace Poincare::NumericSolver
