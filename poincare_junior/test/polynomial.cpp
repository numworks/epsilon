#include "print.h"
#include <poincare_junior/src/expression/polynomial.h>
#include <poincare_junior/src/expression/constructor.h>

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
      /* π^3 + 3*π^2*e + 3*π*e^2 + e^3 */ Add(Pow(π_e, 3_e), Mult(3_e, Pow(π_e, 2_e), "e"_e), Mult(3_e, Pow("e"_e, 2_e), π_e), Pow("e"_e, 3_e)),
      /* variables = {π, e} */ Set(π_e, "e"_e),
      /* polynomial */ // TODO Pol(exponentsPi, π_e, 1_e, Pol(exponentsE0, "e"_e, 3_e), Pol(exponentsE1, "e"_e, 3_e), Pol(exponentsE2, "e"_e, 1_e))
      /* polynomial */ Pol(Exponents<3, 2, 1, 0>(), π_e, 1_e, Pol(Exponents<1>(), "e"_e, Mult(3_e, 1_e, 1_e)), Pol(Exponents<2>(), "e"_e, Mult(3_e, 1_e, 1_e)), Pol(Exponents<3>(), "e"_e, 1_e))
    );

  assert_polynomial_is_parsed(
      /* 42 */ 42_e,
      /* variables = {} */ Set(),
      /* polynomial */ 42_e
    );

  assert_polynomial_is_parsed(
      /* π^1.2 */ Pow(π_e, 1.2_e),
      /* variables = {π^1.2} */ Set(Pow(π_e, 1.2_e)),
      /* polynomial */ Pol(Exponents<1>(), Pow(π_e, 1.2_e), 1_e)
    );

  // TODO: parse polynomial with float coefficients?
}

QUIZ_CASE(pcj_polynomial_operations) {
  /* A = x^2 + 3*x*y + y + 1 */
  Node polA = Pol(Exponents<2, 1, 0>(), "x"_e, 1_e, Pol(Exponents<1>(),  "y"_e, 3_e), Pol(Exponents<1, 0>(), "y"_e, 1_e, 1_e));
  /* B = x^3 + 2*x*y^2 + 7*x*y + 23 */
  Node polB = Pol(Exponents<3, 1, 0>(), "x"_e, 1_e, Pol(Exponents<2, 1>(),  "y"_e, 2_e, 7_e), 23_e);

  /* A + B = x^3 + x^2 + 2*x*y^2 + 10*x*y + y + 24 */
  // TODO once basicReduction implemented assert_trees_are_equal(Polynomial::Addition(polA, polB), EditionReference(Pol(Exponents<3, 2, 1, 0>(), "x"_e, 1_e, 1_e, Pol(Exponents<2, 1>(), "y"_e, 2_e, 10_e), Pol(Exponents<1, 0>(), "y"_e, 1_e, 24_e))));
  assert_trees_are_equal(Polynomial::Addition(EditionReference(polA), EditionReference(polB)), EditionReference(Pol(Exponents<3, 2, 1, 0>(), "x"_e, 1_e, 1_e, Pol(Exponents<2, 1>(), "y"_e, 2_e, Add(3_e, 7_e)), Pol(Exponents<1, 0>(), "y"_e, 1_e, Add(1_e, 23_e)))));
  CachePool::sharedCachePool()->editionPool()->flush();

  /* A * B = x^5 + 3yx^4 + (2y^2+8y+1)*x^3 + (6y^3+21y^2+23)x^2 + (2y^3+9y^2+ 76y)x + 23y + 23 */
  // TODO once basicReduction assert_trees_are_equal(Polynomial::Multiplication(EditionReference(polA), EditionReference(polB)), EditionReference(Pol(Exponents<5, 4, 3, 2, 1, 0>(), "x"_e, 1_e, Pol(Exponents<1>(), "y"_e, 3_e), Pol(Exponents<2, 1, 0>(), "y"_e, 2_e, 8_e, 1_e), Pol(Exponents<3, 2, 0>(), "y"_e, 6_e, "21"_e, 23_e), Pol(Exponents<3, 2, 1>(), "y"_e, 2_e, 9_e, 76_e, Pol(Exponents<1, 0>(), "y"_e, 23_e, 23_e))));
  assert_trees_are_equal(Polynomial::Multiplication(EditionReference(polA), EditionReference(polB)), EditionReference(Pol(Exponents<5, 4, 3, 2, 1, 0>(), "x"_e, Mult(1_e, 1_e), Pol(Exponents<1>(), "y"_e, Mult(3_e, 1_e)), Pol(Exponents<2, 1, 0>(), "y"_e, Mult(2_e, 1_e), Add( Mult(1_e, 1_e), Mult(7_e, 1_e)), Mult(1_e, 1_e)), Pol(Exponents<3, 2, 0>(), "y"_e,  Mult(3_e, 2_e), Mult(3_e, 7_e), Mult(1_e, 23_e)), Pol(Exponents<3, 2, 1>(), "y"_e, Mult(1_e, 2_e), Add(Mult(1_e, 2_e), Mult(1_e, 7_e)), Add(Mult(1_e, 7_e), Mult(3_e, 23_e))), Pol(Exponents<1, 0>(), "y"_e, Mult(1_e, 23_e), Mult(1_e, 23_e)))));
  CachePool::sharedCachePool()->editionPool()->flush();

  /* Test variable order:
   * (y^2) + ((y+1)x + 1 = (y+1)x + y^2 + 1 */
  assert_trees_are_equal(Polynomial::Addition(EditionReference(Pol(Exponents<2>(), "y"_e, 1_e)), EditionReference(Pol(Exponents<1,0>(), "x"_e, Pol(Exponents<1,0>(), "y"_e, 1_e, 1_e), 1_e))), Pol(Exponents<1, 0>(), "x"_e, Pol(Exponents<1,0>(), "y"_e, 1_e, 1_e), Pol(Exponents<2,0>(), "y"_e, 1_e, 1_e)));
  CachePool::sharedCachePool()->editionPool()->flush();

  // A = x^2y^2 + x
  polA = Pol(Exponents<2,1>(), "x"_e, Pol(Exponents<2>(), "y"_e, 1_e), 1_e);
  // B = xy + 1
  polB = Pol(Exponents<1,0>(), "x"_e, Pol(Exponents<1>(), "y"_e, 1_e), 1_e);
  // TODO implement basicReduction to make it works
  //auto [quotient, remainder] = Polynomial::PseudoDivision(polA, polB);
  //assert_trees_are_equal(quotient, Pol(Exponents<1, 0>(), "x"_e, Pol(Exponents<1>(), "y"_e, 1_e), Sub(0_e, 1_e)));
  //assert_trees_are_equal(remainder, Pol(Exponents<1, 0>(), "x"_e, 1_e, 1_e));
}
