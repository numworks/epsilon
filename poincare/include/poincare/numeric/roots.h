#ifndef POINCARE_NUMERIC_ROOTS_H
#define POINCARE_NUMERIC_ROOTS_H

#include <poincare/old/computation_context.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/rational.h>
#include <poincare/src/expression/sign.h>
#include <poincare/src/memory/tree.h>

namespace Poincare {

namespace Internal {

class Roots {
 public:
  // Return the only root.
  static Tree* Linear(const Tree* a, const Tree* b);
  /* Return a list of one or two roots, in decreasing order
   *   delta can be provided or will be computed */
  static Tree* Quadratic(const Tree* a, const Tree* b, const Tree* c,
                         const Tree* discriminant = nullptr);
  static Tree* QuadraticDiscriminant(const Tree* a, const Tree* b,
                                     const Tree* c);

  static Tree* Cubic(const Tree* a, const Tree* b, const Tree* c, const Tree* d,
                     const Tree* discriminant = nullptr);
  static Tree* CubicDiscriminant(const Tree* a, const Tree* b, const Tree* c,
                                 const Tree* d);

 private:
  constexpr static int k_maxNumberOfNodesBeforeApproximatingDelta = 16;

  static constexpr KTree k_cubeRootOfUnity1 = KMult(
      KPow(2_e, -1_e), KAdd(-1_e, KMult(KPow(3_e, KPow(2_e, -1_e)), i_e)));

  static constexpr KTree k_cubeRootOfUnity2 =
      KMult(KPow(2_e, -1_e),
            KAdd(-1_e, KMult(-1_e, KPow(3_e, KPow(2_e, -1_e)), i_e)));

  static Tree* CubicRootsKnowingNonZeroRoot(const Tree* a, const Tree* b,
                                            const Tree* c, const Tree* d,
                                            const Tree* r);

  static Tree* EvaluatePolynomialAtValue(const Tree* value, const Tree* a,
                                         const Tree* b, const Tree* c,
                                         const Tree* d);

  static bool IsRoot(const Tree* root, const Tree* a, const Tree* b,
                     const Tree* c, const Tree* d);

  static Tree* ReducePolynomial(const Tree* coefficients, int degree,
                                const Tree* parameter,
                                const ReductionContext& reductionContext);
  static Rational ReduceRationalPolynomial(const Rational* coefficients,
                                           int degree, Rational parameter);

  static Tree* CubicRootsNullLastCoefficient(const Tree* a, const Tree* b,
                                             const Tree* c);

  static Tree* CubicRootsNullSecondAndThirdCoefficients(const Tree* a,
                                                        const Tree* d);

  static Tree* RationalRootSearch(const Tree* coefficients, int degree,
                                  const ReductionContext& reductionContext);
  static Tree* SumRootSearch(const Tree* coefficients, int degree,
                             int relevantCoefficient,
                             const ReductionContext& reductionContext);
  static Tree* CardanoNumber(const Tree* delta0, const Tree* delta1,
                             bool* approximate,
                             const ReductionContext& reductionContext);

#if 0
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
               .isNull(reductionContext.context()) == OMG::Troolean::True;
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
#endif
};

}  // namespace Internal

using Roots = Internal::Roots;

}  // namespace Poincare

#endif
