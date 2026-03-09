#pragma once

#include <poincare/expression.h>
#include <poincare/k_tree.h>

namespace Poincare::Internal {
class Tree;
}  // namespace Poincare::Internal

namespace Poincare {

/* Expressions that have been projected, systematically reduced, and arranged
 * for better analysis. */
class AnalysisExpression : public Expression {
 public:
  /* Static builders */
  static AnalysisExpression Builder(const Internal::Tree* tree);
  // Eat the tree
  static AnalysisExpression Builder(Internal::Tree* tree);
  template <Internal::KTrees::KTreeConcept T>
  static AnalysisExpression Builder(T x) {
    return Builder(static_cast<const Internal::Tree*>(x));
  }

  /* General helpers */
  AnalysisExpression cloneChildAtIndex(int i) const;
  AnalysisExpression clone() const {
    PoolHandle clone = PoolHandle::clone();
    return static_cast<AnalysisExpression&>(clone);
  }

  /* Other helpers */
  int polynomialDegree(const char* symbolName) const;
  /* Return a list of the polynomial coefficients. Polynomial degree is list
   * length minus one. */
  AnalysisExpression getPolynomialReducedCoefficients(
      const char* symbolName, bool keepDependencies = false) const;
  /* Approximate to a real scalar. AnalysisExpression aren't optimal for
   * approximation, they can induce big approximation errors. */
  template <typename T>
  T approximateSystemToRealScalar() const;
};

}  // namespace Poincare
