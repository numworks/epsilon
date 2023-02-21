#ifndef POINCARE_POLYNOMIAL_H
#define POINCARE_POLYNOMIAL_H

#include <poincare/expression.h>
#include <poincare/rational.h>

namespace Poincare {

class Polynomial {
 public:
  static int LinearPolynomialRoots(Expression a, Expression b, Expression* root,
                                   ReductionContext reductionContext,
                                   bool beautifyRoots = true);

  static int QuadraticPolynomialRoots(Expression a, Expression b, Expression c,
                                      Expression* root1, Expression* root2,
                                      Expression* delta,
                                      ReductionContext reductionContext,
                                      bool approximateSolutions = false,
                                      bool beautifyRoots = true);

  static int CubicPolynomialRoots(Expression a, Expression b, Expression c,
                                  Expression d, Expression* root1,
                                  Expression* root2, Expression* root3,
                                  Expression* delta,
                                  ReductionContext reductionContext,
                                  bool* approximateSolutions = nullptr,
                                  bool beautifyRoots = true);

 private:
  constexpr static int k_maxNumberOfNodesBeforeApproximatingDelta = 16;
  static Expression ReducePolynomial(const Expression* coefficients, int degree,
                                     Expression parameter,
                                     const ReductionContext& reductionContext);
  static Rational ReduceRationalPolynomial(const Rational* coefficients,
                                           int degree, Rational parameter);
  static bool IsRoot(const Expression* coefficients, int degree,
                     Expression root,
                     const ReductionContext& reductionContext) {
    return ReducePolynomial(coefficients, degree, root, reductionContext)
               .isNull(reductionContext.context()) == TrinaryBoolean::True;
  }
  static Expression RationalRootSearch(
      const Expression* coefficients, int degree,
      const ReductionContext& reductionContext);
  static Expression SumRootSearch(const Expression* coefficients, int degree,
                                  int relevantCoefficient,
                                  const ReductionContext& reductionContext);
  static Expression CardanoNumber(Expression delta0, Expression delta1,
                                  bool* approximate,
                                  const ReductionContext& reductionContext);
};

}  // namespace Poincare

#endif
