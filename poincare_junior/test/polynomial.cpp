#include "helper.h"
#include <poincare_junior/src/expression/polynomial.h>
#include <poincare_junior/src/expression/k_creator.h>

using namespace PoincareJ;

void assert_polynomial_is_parsed(const Node node, const Node expectedVariables, const Node expectedPolynomial) {
  CachePool::sharedCachePool()->editionPool()->flush();
  EditionReference variables = PolynomialParser::GetVariables(node);
  assert_trees_are_equal(variables, expectedVariables);
  EditionReference ref(node);
  EditionReference polynomial = PolynomialParser::RecursivelyParse(ref, variables);
  assert_trees_are_equal(polynomial, expectedPolynomial);
}

QUIZ_CASE(pcj_polynomial_parsing) {
  assert_polynomial_is_parsed(
      /* π^3 + 3*π^2*e + 3*π*e^2 + e^3 */ KAdd(KPow(π_e, 3_e), KMult(3_e, KPow(π_e, 2_e), "e"_e), KMult(3_e, KPow("e"_e, 2_e), π_e), KPow("e"_e, 3_e)),
      /* variables = {π, e} */ KSet(π_e, "e"_e),
      /* polynomial */ // TODO KPol(exponentsPi, π_e, 1_e, KPol(exponentsE0, "e"_e, 3_e), KPol(exponentsE1, "e"_e, 3_e), KPol(exponentsE2, "e"_e, 1_e))
      /* polynomial */ KPol(Exponents<3, 2, 1, 0>(), π_e, 1_e, KPol(Exponents<1>(), "e"_e, KMult(3_e, 1_e, 1_e)), KPol(Exponents<2>(), "e"_e, KMult(3_e, 1_e, 1_e)), KPol(Exponents<3>(), "e"_e, 1_e))
    );

  assert_polynomial_is_parsed(
      /* 42 */ 42_e,
      /* variables = {} */ KSet(),
      /* polynomial */ 42_e
    );

  assert_polynomial_is_parsed(
      /* π^1.2 */ KPow(π_e, 1.2_e),
      /* variables = {π^1.2} */ KSet(KPow(π_e, 1.2_e)),
      /* polynomial */ KPol(Exponents<1>(), KPow(π_e, 1.2_e), 1_e)
    );

  // TODO: parse polynomial with float coefficients?
}

QUIZ_CASE(pcj_polynomial_operations) {
  /* A = x^2 + 3*x*y + y + 1 */
  Node polA = KPol(Exponents<2, 1, 0>(), "x"_e, 1_e, KPol(Exponents<1>(),  "y"_e, 3_e), KPol(Exponents<1, 0>(), "y"_e, 1_e, 1_e));
  /* B = x^3 + 2*x*y^2 + 7*x*y + 23 */
  Node polB = KPol(Exponents<3, 1, 0>(), "x"_e, 1_e, KPol(Exponents<2, 1>(),  "y"_e, 2_e, 7_e), 23_e);

  /* A + B = x^3 + x^2 + 2*x*y^2 + 10*x*y + y + 24 */
  // TODO once basicReduction implemented assert_trees_are_equal(Polynomial::KAddition(polA, polB), EditionReference(KPol(Exponents<3, 2, 1, 0>(), "x"_e, 1_e, 1_e, KPol(Exponents<2, 1>(), "y"_e, 2_e, 10_e), KPol(Exponents<1, 0>(), "y"_e, 1_e, 24_e))));
  assert_trees_are_equal(Polynomial::Addition(EditionReference(polA), EditionReference(polB)), EditionReference(KPol(Exponents<3, 2, 1, 0>(), "x"_e, 1_e, 1_e, KPol(Exponents<2, 1>(), "y"_e, 2_e, KAdd(3_e, 7_e)), KPol(Exponents<1, 0>(), "y"_e, 1_e, KAdd(1_e, 23_e)))));
  CachePool::sharedCachePool()->editionPool()->flush();

  /* A * B = x^5 + 3yx^4 + (2y^2+8y+1)*x^3 + (6y^3+21y^2+23)x^2 + (2y^3+9y^2+ 76y)x + 23y + 23 */
  // TODO once basicReduction assert_trees_are_equal(Polynomial::KMultiplication(EditionReference(polA), EditionReference(polB)), EditionReference(KPol(Exponents<5, 4, 3, 2, 1, 0>(), "x"_e, 1_e, KPol(Exponents<1>(), "y"_e, 3_e), KPol(Exponents<2, 1, 0>(), "y"_e, 2_e, 8_e, 1_e), KPol(Exponents<3, 2, 0>(), "y"_e, 6_e, "21"_e, 23_e), KPol(Exponents<3, 2, 1>(), "y"_e, 2_e, 9_e, 76_e, KPol(Exponents<1, 0>(), "y"_e, 23_e, 23_e))));
  assert_trees_are_equal(Polynomial::Multiplication(EditionReference(polA), EditionReference(polB)), EditionReference(KPol(Exponents<5, 4, 3, 2, 1, 0>(), "x"_e, KMult(1_e, 1_e), KPol(Exponents<1>(), "y"_e, KMult(3_e, 1_e)), KPol(Exponents<2, 1, 0>(), "y"_e, KMult(2_e, 1_e), KAdd( KMult(1_e, 1_e), KMult(7_e, 1_e)), KMult(1_e, 1_e)), KPol(Exponents<3, 2, 0>(), "y"_e,  KMult(3_e, 2_e), KMult(3_e, 7_e), KMult(1_e, 23_e)), KPol(Exponents<3, 2, 1>(), "y"_e, KMult(1_e, 2_e), KAdd(KMult(1_e, 2_e), KMult(1_e, 7_e)), KAdd(KMult(1_e, 7_e), KMult(3_e, 23_e))), KPol(Exponents<1, 0>(), "y"_e, KMult(1_e, 23_e), KMult(1_e, 23_e)))));
  CachePool::sharedCachePool()->editionPool()->flush();

  /* Test variable order:
   * (y^2) + ((y+1)x + 1 = (y+1)x + y^2 + 1 */
  assert_trees_are_equal(Polynomial::Addition(EditionReference(KPol(Exponents<2>(), "y"_e, 1_e)), EditionReference(KPol(Exponents<1,0>(), "x"_e, KPol(Exponents<1,0>(), "y"_e, 1_e, 1_e), 1_e))), KPol(Exponents<1, 0>(), "x"_e, KPol(Exponents<1,0>(), "y"_e, 1_e, 1_e), KPol(Exponents<2,0>(), "y"_e, 1_e, 1_e)));
  CachePool::sharedCachePool()->editionPool()->flush();

  // A = x^2y^2 + x
  polA = KPol(Exponents<2,1>(), "x"_e, KPol(Exponents<2>(), "y"_e, 1_e), 1_e);
  // B = xy + 1
  polB = KPol(Exponents<1,0>(), "x"_e, KPol(Exponents<1>(), "y"_e, 1_e), 1_e);
  // TODO implement basicReduction to make it works
  //auto [quotient, remainder] = Polynomial::PseudoDivision(polA, polB);
  //assert_trees_are_equal(quotient, KPol(Exponents<1, 0>(), "x"_e, KPol(Exponents<1>(), "y"_e, 1_e), KSub(0_e, 1_e)));
  //assert_trees_are_equal(remainder, KPol(Exponents<1, 0>(), "x"_e, 1_e, 1_e));
}
