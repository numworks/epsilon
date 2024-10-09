#ifndef POINCARE_NUMERIC_ROOTS_H
#define POINCARE_NUMERIC_ROOTS_H

#include <poincare/old/computation_context.h>
#include <poincare/sign.h>
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

  /* Return a list of one or two roots, in decreasing order.
   * Delta can be provided or will be computed. */
  static Tree* Quadratic(const Tree* a, const Tree* b, const Tree* c,
                         const Tree* discriminant = nullptr);
  static Tree* QuadraticDiscriminant(const Tree* a, const Tree* b,
                                     const Tree* c);

  /* Return a list of at most three roots, in decreasing order.
   * Delta can be provided or will be computed. */
  static Tree* Cubic(const Tree* a, const Tree* b, const Tree* c, const Tree* d,
                     const Tree* preComputedDiscriminant = nullptr);
  static Tree* CubicDiscriminant(const Tree* a, const Tree* b, const Tree* c,
                                 const Tree* d);
  /* The approximation of cubic roots requires special attention. We have
   * information on the number of real roots, looking at the discriminant
   * sign. Approximation errors adding a very small imaginary part can be
   * removed using this information. */
  /* WARNING: this function assumes that all the cubic coefficients are real.
   * Otherwise, we do not have any information on the "realness" of roots, and
   * we can simply call the usual approximation function. */
  static Tree* ApproximateRootsOfRealCubic(const Tree* roots,
                                           const Tree* discriminant);

 private:
  static constexpr KTree k_squareRootOfThree = KExp(KMult(1_e / 2_e, KLn(3_e)));

  // (-1 + i√(3)) / 2
  static constexpr KTree k_cubeRootOfUnity1 =
      KMult(1_e / 2_e, KAdd(-1_e, KMult(k_squareRootOfThree, i_e)));

  // (-1 - i√(3)) / 2
  static constexpr KTree k_cubeRootOfUnity2 =
      KMult(1_e / 2_e, KAdd(-1_e, KMult(-1_e, k_squareRootOfThree, i_e)));

  static Tree* PolynomialEvaluation(const Tree* value, const Tree* a,
                                    const Tree* b, const Tree* c,
                                    const Tree* d);

  static bool IsRoot(const Tree* value, const Tree* a, const Tree* b,
                     const Tree* c, const Tree* d);

  static Tree* CubicRootsNullSecondAndThirdCoefficients(const Tree* a,
                                                        const Tree* d);

  static Tree* SimpleRootSearch(const Tree* a, const Tree* b, const Tree* c,
                                const Tree* d);
  static Tree* RationalRootSearch(const Tree* a, const Tree* b, const Tree* c,
                                  const Tree* d);
  static Tree* SumRootSearch(const Tree* a, const Tree* b, const Tree* c,
                             const Tree* d);

  static Tree* CubicRootsKnowingNonZeroRoot(const Tree* a, const Tree* b,
                                            const Tree* c, const Tree* d,
                                            Tree* r);

  static Tree* CardanoMethod(const Tree* a, const Tree* b, const Tree* c,
                             const Tree* d, const Tree* delta);
  static Tree* CubicRootsNullDiscriminant(const Tree* a, const Tree* b,
                                          const Tree* c, const Tree* d);
  static Tree* Delta0(const Tree* a, const Tree* b, const Tree* c);
  static Tree* Delta1(const Tree* a, const Tree* b, const Tree* c,
                      const Tree* d);
  static Tree* CardanoNumber(const Tree* delta0, const Tree* delta1);
  static Tree* CardanoRoot(const Tree* a, const Tree* b, const Tree* cardano,
                           const Tree* delta0, uint8_t k);
};

}  // namespace Internal

using Roots = Internal::Roots;

}  // namespace Poincare

#endif
