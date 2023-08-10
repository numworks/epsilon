#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/expression/polynomial.h>

#include "helper.h"

using namespace PoincareJ;

void assert_polynomial_is_parsed(const Tree* node,
                                 const Tree* expectedVariables,
                                 const Tree* expectedPolynomial) {
  SharedEditionPool->flush();
  EditionReference variables = PolynomialParser::GetVariables(node);
  assert_trees_are_equal(variables, expectedVariables);
  EditionReference ref(node->clone());
  EditionReference polynomial =
      PolynomialParser::RecursivelyParse(ref, variables);
  assert_trees_are_equal(polynomial, expectedPolynomial);
}

QUIZ_CASE(pcj_polynomial_parsing) {
  assert_polynomial_is_parsed(
      /* 42 */ 42_e,
      /* variables = {} */ KSet(),
      /* polynomial */ 42_e);
  assert_polynomial_is_parsed(
      /* π^3 + 3*π^2*e + 3*π*e^2 + e^3 */ KAdd(
          KPow(π_e, 3_e), KMult(3_e, KPow(π_e, 2_e), "e"_e),
          KMult(3_e, KPow("e"_e, 2_e), π_e), KPow("e"_e, 3_e)),
      /* variables = {π, e} */ KSet(π_e, "e"_e),
      /* polynomial */
      KPol(Exponents<3, 2, 1, 0>(), π_e, 1_e, KPol(Exponents<1>(), "e"_e, 3_e),
           KPol(Exponents<2>(), "e"_e, 3_e), KPol(Exponents<3>(), "e"_e, 1_e)));
  // TODO: parse polynomial with float coefficients?
}

QUIZ_CASE(pcj_polynomial_variables) {
  assert_trees_are_equal(
      PolynomialParser::GetVariables(KMult(2_e, "x"_e, "y"_e)),
      KSet("x"_e, "y"_e));
  assert_trees_are_equal(
      PolynomialParser::GetVariables(KAdd(KPow("x"_e, 2_e), KLn("x"_e))),
      KSet("x"_e, KLn("x"_e)));
  assert_trees_are_equal(PolynomialParser::GetVariables(
                             KAdd(KPow("x"_e, 2_e), KPow("y"_e, 2_e),
                                  KMult(-1_e, KPow(KAdd("x"_e, "y"_e), 2_e)))),
                         KSet("x"_e, "y"_e, KAdd("x"_e, "y"_e)));
  assert_trees_are_equal(
      PolynomialParser::GetVariables(KAdd("x"_e, KPow("y"_e, -1_e))),
      KSet("x"_e, KPow("y"_e, -1_e)));
  assert_trees_are_equal(PolynomialParser::GetVariables(KMult(
                             KExp(KMult(KHalf, KLn(2_e))), "x"_e, i_e, π_e)),
                         KSet(π_e, i_e, "x"_e));
}

QUIZ_CASE(pcj_polynomial_operations) {
  /* A = x^2 + 3*x*y + y + 1 */
  const Tree* polA =
      KPol(Exponents<2, 1, 0>(), "x"_e, 1_e, KPol(Exponents<1>(), "y"_e, 3_e),
           KPol(Exponents<1, 0>(), "y"_e, 1_e, 1_e));
  /* B = x^3 + 2*x*y^2 + 7*x*y + 23 */
  const Tree* polB = KPol(Exponents<3, 1, 0>(), "x"_e, 1_e,
                          KPol(Exponents<2, 1>(), "y"_e, 2_e, 7_e), 23_e);

  /* A + B = x^3 + x^2 + 2*x*y^2 + 10*x*y + y + 24 */
  assert_trees_are_equal(
      Polynomial::Addition(polA->clone(), polB->clone()),
      EditionReference(KPol(Exponents<3, 2, 1, 0>(), "x"_e, 1_e, 1_e,
                            KPol(Exponents<2, 1>(), "y"_e, 2_e, 10_e),
                            KPol(Exponents<1, 0>(), "y"_e, 1_e, 24_e))));
  SharedEditionPool->flush();

  /* A * B = x^5 + 3yx^4 + (2y^2+8y+1)*x^3 + (6y^3+21y^2+23)x^2 +
  (2y^3+9y^2+
   * 76y)x + 23y + 23 */
  assert_trees_are_equal(
      Polynomial::Multiplication(EditionReference(polA->clone()),
                                 EditionReference(polB->clone())),
      EditionReference(KPol(Exponents<5, 4, 3, 2, 1, 0>(), "x"_e, 1_e,
                            KPol(Exponents<1>(), "y"_e, 3_e),
                            KPol(Exponents<2, 1, 0>(), "y"_e, 2_e, 8_e, 1_e),
                            KPol(Exponents<3, 2, 0>(), "y"_e, 6_e, 21_e, 23_e),
                            KPol(Exponents<3, 2, 1>(), "y"_e, 2_e, 9_e, 76_e),
                            KPol(Exponents<1, 0>(), "y"_e, 23_e, 23_e))));
  SharedEditionPool->flush();

  /* Test variable order:
   * (y^2) + ((y+1)x + 1 = (y+1)x + y^2 + 1 */
  assert_trees_are_equal(
      Polynomial::Addition(EditionReference(KPol(Exponents<2>(), "y"_e, 1_e)),
                           EditionReference(KPol(
                               Exponents<1, 0>(), "x"_e,
                               KPol(Exponents<1, 0>(), "y"_e, 1_e, 1_e), 1_e))),
      KPol(Exponents<1, 0>(), "x"_e, KPol(Exponents<1, 0>(), "y"_e, 1_e, 1_e),
           KPol(Exponents<2, 0>(), "y"_e, 1_e, 1_e)));
  SharedEditionPool->flush();

  // A = x^2y^2 + y
  polA = KPol(Exponents<2, 0>(), "x"_e, KPol(Exponents<2>(), "y"_e, 1_e),
              KPol(Exponents<1>(), "y"_e, 1_e));
  // B = xy + 1
  polB = KPol(Exponents<1, 0>(), "x"_e, KPol(Exponents<1>(), "y"_e, 1_e), 1_e);
  // A / B = (xy + 1)*(xy - 1) + y + 1
  auto [quotient, remainder] =
      Polynomial::PseudoDivision(polA->clone(), polB->clone());
  assert_trees_are_equal(
      quotient,
      KPol(Exponents<1, 0>(), "x"_e, KPol(Exponents<1>(), "y"_e, 1_e), -1_e));
  assert_trees_are_equal(remainder, KPol(Exponents<1, 0>(), "y"_e, 1_e, 1_e));
}
