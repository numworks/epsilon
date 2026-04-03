#pragma once

#include <poincare/src/expression/approximation.h>
#include <poincare/tree.h>

namespace Poincare::NumericSolver {

template <typename TFloat>
class ExpressionEvaluator {
 public:
  /* NOTE: e should be a prepared function */
  explicit ExpressionEvaluator(const Internal::Tree* e)
      : m_tree(e),
        m_isRandomized(e &&
                       e->hasDescendantSatisfying([](const Internal::Tree* t) {
                         return t->isRandomized();
                       })) {}

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

  const Internal::Tree* tree() const { return m_tree; }

 private:
  const Internal::Tree* m_tree;
  bool m_isRandomized;
};

template <typename TFloat>
using FunctionEvaluation = TFloat (*)(TFloat, const void*);

template <typename TFloat>
class LambdaEvaluator {
 public:
  LambdaEvaluator(FunctionEvaluation<TFloat> function, const void* auxiliary)
      : m_function(function), m_auxiliary(auxiliary) {}

  TFloat operator()(TFloat x) const { return m_function(x, m_auxiliary); }

  const void* aux() const { return m_auxiliary; }

 private:
  FunctionEvaluation<TFloat> m_function;
  const void* m_auxiliary;
};

}  // namespace Poincare::NumericSolver
