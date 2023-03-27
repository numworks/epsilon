#ifndef POINCARE_SOLVER_H
#define POINCARE_SOLVER_H

#include <math.h>
#include <poincare/expression.h>
#include <poincare/float.h>

namespace Poincare {

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

  typedef T (*FunctionEvaluation)(T, const void *);
  typedef Interest (*BracketTest)(Coordinate2D<T>, Coordinate2D<T>,
                                  Coordinate2D<T>, const void *);
  typedef Coordinate2D<T> (*HoneResult)(FunctionEvaluation, const void *, T, T,
                                        Interest, T, TrinaryBoolean);
  typedef bool (*DiscontinuityEvaluation)(T, T, const void *);

  constexpr static T k_relativePrecision = Float<T>::Epsilon();
  constexpr static T k_minimalAbsoluteStep =
      2. * Helpers::SquareRoot(2. * k_relativePrecision);

  static T NullTolerance(T x) {
    return std::max(k_relativePrecision,
                    Helpers::SquareRoot(k_relativePrecision) * std::fabs(x));
  }
  static T MaximalStep(T intervalAmplitude);

  // BracketTest default implementations
  constexpr static Interest BoolToInterest(bool v, Interest t,
                                           Interest f = Interest::None) {
    return v ? t : f;
  }
  static Interest OddRootInBracket(Coordinate2D<T> a, Coordinate2D<T> b,
                                   Coordinate2D<T> c, const void *) {
    return BoolToInterest((a.y() < k_zero && k_zero < c.y()) ||
                              (c.y() < k_zero && k_zero < a.y()),
                          Interest::Root);
  }
  static Interest EvenOrOddRootInBracket(Coordinate2D<T> a, Coordinate2D<T> b,
                                         Coordinate2D<T> c, const void *);
  static Interest MinimumInBracket(Coordinate2D<T> a, Coordinate2D<T> b,
                                   Coordinate2D<T> c, const void *) {
    return BoolToInterest(b.y() < a.y() && b.y() < c.y(),
                          Interest::LocalMinimum);
  }
  static Interest MaximumInBracket(Coordinate2D<T> a, Coordinate2D<T> b,
                                   Coordinate2D<T> c, const void *) {
    return BoolToInterest(a.y() < b.y() && c.y() < b.y(),
                          Interest::LocalMaximum);
  }
  static Interest UndefinedInBracket(Coordinate2D<T> a, Coordinate2D<T> b,
                                     Coordinate2D<T> c, const void *) {
    return BoolToInterest((std::isfinite(a.y()) && std::isnan(c.y())) ||
                              (std::isfinite(c.y()) && std::isnan(a.y())),
                          Interest::Discontinuity);
  }

  /* Arguments beyond xEnd are only required if the Solver manipulates
   * Expression. */
  Solver(T xStart, T xEnd, const char *unknown = nullptr,
         Context *context = nullptr,
         Preferences::ComplexFormat complexFormat =
             Preferences::ComplexFormat::Cartesian,
         Preferences::AngleUnit angleUnit = Preferences::AngleUnit::Radian);

  T start() const { return m_xStart; }
  T end() const { return m_xEnd; }
  Interest lastInterest() const { return m_lastInterest; }
  Coordinate2D<T> result() const {
    return lastInterest() == Interest::None
               ? Coordinate2D<T>(k_NAN, k_NAN)
               : Coordinate2D<T>(start(), m_yResult);
  }

  /* These methods will return the solution in ]xStart,xEnd[ (or ]xEnd,xStart[)
   * closest to xStart, or NAN if it does not exist. */
  Coordinate2D<T> next(const Expression &e, BracketTest test, HoneResult hone);
  Coordinate2D<T> next(FunctionEvaluation f, const void *aux, BracketTest test,
                       HoneResult hone,
                       DiscontinuityEvaluation discontinuityTest = nullptr);
  Coordinate2D<T> nextRoot(const Expression &e);
  Coordinate2D<T> nextRoot(FunctionEvaluation f, const void *aux) {
    return next(f, aux, EvenOrOddRootInBracket, CompositeBrentForRoot);
  }
  Coordinate2D<T> nextMinimum(const Expression &e);
  Coordinate2D<T> nextMaximum(const Expression &e) {
    return next(e, MaximumInBracket, SafeBrentMaximum);
  }
  /* Caller of nextIntersection may provide a place to store the difference
   * between the two expressions, in case the method needs to be called several
   * times in a row. */
  Coordinate2D<T> nextIntersection(const Expression &e1, const Expression &e2,
                                   Expression *memoizedDifference = nullptr);
  /* Stretch the interval to include the previous bounds. This allows finding
   * solutions in [xStart,xEnd], as otherwise all resolution is done on an open
   * interval. */
  void stretch();
  void setSearchStep(T step) { m_maximalXStep = step; }

 private:
  struct FunctionEvaluationParameters {
    Context *context;
    const char *unknown;
    Expression expression;
    Preferences::ComplexFormat complexFormat;
    Preferences::AngleUnit angleUnit;
  };

  constexpr static T k_NAN = static_cast<T>(NAN);
  constexpr static T k_zero = static_cast<T>(0.);
  /* We use k_minimalPracticalStep (10^-6) when stepping around zero instead of
   * k_minimalAbsoluteStep (~10^-8), to avoid wasting time with too many very
   * precise computations. */
  constexpr static T k_minimalPracticalStep =
      std::max(static_cast<T>(1e-6), k_minimalAbsoluteStep);
  constexpr static T k_absolutePrecision =
      k_relativePrecision * k_minimalAbsoluteStep;

  static Coordinate2D<T> SafeBrentMinimum(FunctionEvaluation f, const void *aux,
                                          T xMin, T xMax, Interest interest,
                                          T precision,
                                          TrinaryBoolean discontinuous);
  static Coordinate2D<T> SafeBrentMaximum(FunctionEvaluation f, const void *aux,
                                          T xMin, T xMax, Interest interest,
                                          T precision,
                                          TrinaryBoolean discontinuous);
  static Coordinate2D<T> CompositeBrentForRoot(FunctionEvaluation f,
                                               const void *aux, T xMin, T xMax,
                                               Interest interest, T precision,
                                               TrinaryBoolean discontinuous);

  /* This filters out discontinuities by comparing the ordinate of the root to
   * the ordinate of the points at abscissas around it. */
  static bool IsOddRoot(Coordinate2D<T> root, FunctionEvaluation f,
                        const void *aux);
  static bool DiscontinuityTest(T x1, T x2, const void *aux);
  static void ExcludeUndefinedFromBracket(Coordinate2D<T> *p1,
                                          Coordinate2D<T> *p2,
                                          Coordinate2D<T> *p3,
                                          FunctionEvaluation f, const void *aux,
                                          T minimalSizeOfInterval);
  static bool FunctionSeemsConstantOnTheInterval(
      Solver<T>::FunctionEvaluation f, const void *aux, T xMin, T xMax);

  T maximalStep() const { return m_maximalXStep; }
  static T MinimalStep(T x, T slope = static_cast<T>(1.));
  bool validSolution(T x) const;
  T nextX(T x, T direction, T slope) const;
  Coordinate2D<T> nextPossibleRootInChild(const Expression &e,
                                          int childIndex) const;
  Coordinate2D<T> nextRootInChildren(const Expression &e,
                                     Expression::ExpressionTestAuxiliary test,
                                     void *aux) const;
  Coordinate2D<T> nextRootInMultiplication(const Expression &m) const;
  Coordinate2D<T> nextRootInAddition(const Expression &m) const;
  Coordinate2D<T> honeAndRoundSolution(
      FunctionEvaluation f, const void *aux, T start, T end, Interest interest,
      HoneResult hone, DiscontinuityEvaluation discontinuityTest);
  void registerSolution(Coordinate2D<T> solution, Interest interest);

  T m_xStart;
  T m_xEnd;
  T m_maximalXStep;
  T m_yResult;
  Context *m_context;
  const char *m_unknown;
  Preferences::ComplexFormat m_complexFormat;
  Preferences::AngleUnit m_angleUnit;
  Interest m_lastInterest;
};

}  // namespace Poincare

#endif
