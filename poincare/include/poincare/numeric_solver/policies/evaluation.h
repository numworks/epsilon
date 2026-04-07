#pragma once

#include <poincare/src/expression/approximation.h>
#include <poincare/tree.h>

namespace Poincare::NumericSolver {

template <typename TFloat>
class LambdaEvaluator {
  /* NOTE: FunctionEvaluation is private to encourage the use of LambdaEvaluator
   * in signatures, instead of a pair (FunctionEvaluation, const void*). */
  using FunctionEvaluation = TFloat (*)(TFloat, const void*);

 public:
  LambdaEvaluator(FunctionEvaluation function, const void* auxiliary)
      : m_function(function), m_auxiliary(auxiliary) {}

  TFloat operator()(TFloat x) const { return m_function(x, m_auxiliary); }

  bool hasFunction() const { return m_function != nullptr; }
  const void* data() const { return m_auxiliary; }

 private:
  FunctionEvaluation m_function;
  const void* m_auxiliary;
};

template <typename TFloat>
class ExpressionEvaluator {
 public:
  /* NOTE: e should be a prepared function */
  explicit ExpressionEvaluator(const Internal::Tree* e)
      : m_tree(e), m_isRandomized(IsExpressionRandomized(e)) {}

  operator LambdaEvaluator<TFloat>() const {
    return {
        [](TFloat x, const void* aux) {
          // FIXME: This conversion drops the isRandomized memoization.
          const Internal::Tree* e = static_cast<const Internal::Tree*>(aux);
          return ExpressionEvaluator<TFloat>(e)(x);
        },
        m_tree,
    };
  }

  TFloat operator()(TFloat x) const {
    assert(m_tree);
    if (m_isRandomized) {
      return TFloat{NAN};
    }

    return Internal::Approximation::To<TFloat>(
        m_tree, x,
        Internal::Approximation::Parameters{
            .policy = {.optimize = true},
            .expressionParameters = Internal::Approximation::Prepared{}});
  }

  const Internal::Tree* data() const { return m_tree; }

 private:
  static bool IsExpressionRandomized(const Internal::Tree* e) {
    return e && e->hasDescendantSatisfying(
                    [](const Internal::Tree* t) { return t->isRandomized(); });
  }

  const Internal::Tree* m_tree;
  bool m_isRandomized;
};

}  // namespace Poincare::NumericSolver
