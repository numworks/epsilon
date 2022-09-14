#ifndef POINCARE_SOLVER_2_H
#define POINCARE_SOLVER_2_H

#include <poincare/expression.h>
#include <poincare/float.h>
#include <math.h>

namespace Poincare {

template<typename T>
class Solver2 {
public:
  enum class Interest : uint8_t {
    None,
    Root,
    LocalMinimum,
    LocalMaximum,
    GlobalMinimum,
    GlobalMaximum,
    Discontinuity,
    Other,
  };

  typedef T (*FunctionEvaluation)(T, const void *);
  typedef Interest (*BracketTest)(T, T, T);
  typedef Coordinate2D<T> (*HoneResult)(FunctionEvaluation, const void *, T, T, Interest, T);

  Solver2(ExpressionNode::ComputationContext * context, T xStart, T xEnd, const char * unknown, T precision = Float<T>::Epsilon());
  Solver2(const Solver2<T> * other);

  T start() const { return m_xStart; }
  T end() const { return m_xEnd; }
  Interest lastInterest() const { return m_lastInterest; }
  Coordinate2D<T> result() const { return lastInterest() == Interest::None ? Coordinate2D<T>(k_NAN, k_NAN) : Coordinate2D<T>(start(), m_yResult); }
  Coordinate2D<T> next(FunctionEvaluation f, const void * aux, BracketTest test, HoneResult hone);
  Coordinate2D<T> next(Expression e, BracketTest test, HoneResult hone);
  Coordinate2D<T> nextRoot(Expression e);

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

  constexpr static Interest BoolToInterest(bool v, Interest t, Interest f = Interest::None) { return v ? t : f; }
  // BracketTest default implementations
  static Interest RootInBracket(T a, T b, T c) { return BoolToInterest((a < k_zero && k_zero < c) || (c < k_zero && k_zero < a), Interest::Root); }

  T nullTolerance() const { /* TODO */ return m_precision; }
  T nextX(T x, T direction) const;
  Coordinate2D<T> nextPossibleRootInChild(Expression e, int childIndex) const;
  Coordinate2D<T> nextRootInMultiplication(Expression m) const;
  void registerSolution(Coordinate2D<T> solution, Interest interest);

  T m_xStart;
  T m_xEnd;
  T m_yResult;
  T m_precision;
  ExpressionNode::ComputationContext * m_context;
  const char * m_unknown;
  Interest m_lastInterest;
};

}

#endif
