#ifndef POINCARE_POLYNOMIAL_H
#define POINCARE_POLYNOMIAL_H

#include "old_expression.h"
#include "rational.h"

namespace Poincare {

class Polynomial {
 public:
  static int LinearPolynomialRoots(OExpression a, OExpression b,
                                   OExpression* root,
                                   ReductionContext reductionContext,
                                   bool beautifyRoots = true);

  static int QuadraticPolynomialRoots(OExpression a, OExpression b,
                                      OExpression c, OExpression* root1,
                                      OExpression* root2, OExpression* delta,
                                      ReductionContext reductionContext,
                                      bool* approximateSolutions = nullptr,
                                      bool beautifyRoots = true);

  static int CubicPolynomialRoots(OExpression a, OExpression b, OExpression c,
                                  OExpression d, OExpression* root1,
                                  OExpression* root2, OExpression* root3,
                                  OExpression* delta,
                                  ReductionContext reductionContext,
                                  bool* approximateSolutions = nullptr,
                                  bool beautifyRoots = true);

 private:
  constexpr static int k_maxNumberOfNodesBeforeApproximatingDelta = 16;
  static OExpression ReducePolynomial(const OExpression* coefficients,
                                      int degree, OExpression parameter,
                                      const ReductionContext& reductionContext);
  static Rational ReduceRationalPolynomial(const Rational* coefficients,
                                           int degree, Rational parameter);
  static bool IsRoot(const OExpression* coefficients, int degree,
                     OExpression root,
                     const ReductionContext& reductionContext) {
    return ReducePolynomial(coefficients, degree, root, reductionContext)
               .isNull(reductionContext.context()) == TrinaryBoolean::True;
  }
  static OExpression RationalRootSearch(
      const OExpression* coefficients, int degree,
      const ReductionContext& reductionContext);
  static OExpression SumRootSearch(const OExpression* coefficients, int degree,
                                   int relevantCoefficient,
                                   const ReductionContext& reductionContext);
  static OExpression CardanoNumber(OExpression delta0, OExpression delta1,
                                   bool* approximate,
                                   const ReductionContext& reductionContext);
};

}  // namespace Poincare

#endif
