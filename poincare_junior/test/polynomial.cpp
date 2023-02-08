#include "print.h"
#include <poincare_junior/src/expression/polynomial.h>
#include <poincare_junior/src/memory/tree_constructor.h>

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
      /* π^3 + 3*π^2*e + 3*π*e^2 + e^3 */ Add(Pow(u'π'_e, "3"_e), Mult("3"_e, Pow(u'π'_e, "2"_e), u'e'_e), Mult("3"_e, Pow(u'e'_e, "2"_e), u'π'_e), Pow(u'e'_e, "3"_e)),
      /* variables = {π, e} */ Set(u'π'_e, u'e'_e),
      /* polynomial */ // TODO Pol(exponentsPi, u'π'_e, "1"_e, Pol(exponentsE0, u'e'_e, "3"_e), Pol(exponentsE1, u'e'_e, "3"_e), Pol(exponentsE2, u'e'_e, "1"_e))
      /* polynomial */ Pol({3, 2, 1, 0}, u'π'_e, "1"_e, Pol({1}, u'e'_e, Mult("3"_e, "1"_e, "1"_e)), Pol({2}, u'e'_e, Mult("3"_e, "1"_e, "1"_e)), Pol({3}, u'e'_e, "1"_e))
    );

  assert_polynomial_is_parsed(
      /* 42 */ "42"_e,
      /* variables = {} */ Set(),
      /* polynomial */ "42"_e
    );

  assert_polynomial_is_parsed(
      /* π^1.2 */ Pow(u'π'_e, 1.2_e),
      /* variables = {π^1.2} */ Set(Pow(u'π'_e, 1.2_e)),
      /* polynomial */ Pol({1}, Pow(u'π'_e, 1.2_e), "1"_e)
    );

  // TODO: parse polynomial with float coefficients?
}

QUIZ_CASE(pcj_polynomial_operations) {
  /* A = x^2 + 3*x*y + y + 1 */
  Node polA = Pol({2, 1, 0}, "x"_e, "1"_e, Pol({1},  "y"_e, "3"_e), Pol({1, 0}, "y"_e, "1"_e, "1"_e));
  /* B = x^3 + 2*x*y^2 + 7*x*y + 23 */
  Node polB = Pol({3, 1, 0}, "x"_e, "1"_e, Pol({2, 1},  "y"_e, "2"_e, "7"_e), "23"_e);

  /* A + B = x^3 + x^2 + 2*x*y^2 + 10*x*y + y + 24 */
  // TODO once basicReduction implemented assert_trees_are_equal(Polynomial::Addition(polA, polB), EditionReference(Pol({3, 2, 1, 0}, "x"_e, "1"_e, "1"_e, Pol({2, 1}, "y"_e, "2"_e, "10"_e), Pol({1, 0}, "y"_e, "1"_e, "24"_e))));
  assert_trees_are_equal(Polynomial::Addition(EditionReference(polA), EditionReference(polB)), EditionReference(Pol({3, 2, 1, 0}, "x"_e, "1"_e, "1"_e, Pol({2, 1}, "y"_e, "2"_e, Add("3"_e, "7"_e)), Pol({1, 0}, "y"_e, "1"_e, Add("1"_e, "23"_e)))));
  CachePool::sharedCachePool()->editionPool()->flush();

  /* A * B = x^5 + 3yx^4 + (2y^2+8y+1)*x^3 + (6y^3+21y^2+23)x^2 + (2y^3+9y^2+ 76y)x + 23y + 23 */
  // TODO once basicReduction assert_trees_are_equal(Polynomial::Multiplication(EditionReference(polA), EditionReference(polB)), EditionReference(Pol({5, 4, 3, 2, 1, 0}, "x"_e, "1"_e, Pol({1}, "y"_e, "3"_e), Pol({2, 1, 0}, "y"_e, "2"_e, "8"_e, "1"_e), Pol({3, 2, 0}, "y"_e, "6"_e, "21"_e, "23"_e), Pol({3, 2, 1}, "y"_e, "2"_e, "9"_e, "76"_e, Pol({1, 0}, "y"_e, "23"_e, "23"_e))));
  assert_trees_are_equal(Polynomial::Multiplication(EditionReference(polA), EditionReference(polB)), EditionReference(Pol({5, 4, 3, 2, 1, 0}, "x"_e, Mult("1"_e, "1"_e), Pol({1}, "y"_e, Mult("3"_e, "1"_e)), Pol({2, 1, 0}, "y"_e, Mult("2"_e, "1"_e), Add( Mult("1"_e, "1"_e), Mult("7"_e, "1"_e)), Mult("1"_e, "1"_e)), Pol({3, 2, 0}, "y"_e,  Mult("3"_e, "2"_e), Mult("3"_e, "7"_e), Mult("1"_e, "23"_e)), Pol({3, 2, 1}, "y"_e, Mult("1"_e, "2"_e), Add(Mult("1"_e, "2"_e), Mult("1"_e, "7"_e)), Add(Mult("1"_e, "7"_e), Mult("3"_e, "23"_e))), Pol({1, 0}, "y"_e, Mult("1"_e, "23"_e), Mult("1"_e, "23"_e)))));
  CachePool::sharedCachePool()->editionPool()->flush();

  /* Test variable order:
   * (y^2) + ((y+1)x + 1 = (y+1)x + y^2 + 1 */
  assert_trees_are_equal(Polynomial::Addition(EditionReference(Pol({2}, "y"_e, "1"_e)), EditionReference(Pol({1,0}, "x"_e, Pol({1,0}, "y"_e, "1"_e, "1"_e), "1"_e))), Pol({1, 0}, "x"_e, Pol({1,0}, "y"_e, "1"_e, "1"_e), Pol({2,0}, "y"_e, "1"_e, "1"_e)));
  CachePool::sharedCachePool()->editionPool()->flush();

  // A = x^2y^2 + x
  polA = Pol({2,1}, "x"_e, Pol({2}, "y"_e, "1"_e), "1"_e);
  // B = xy + 1
  polB = Pol({1,0}, "x"_e, Pol({1}, "y"_e, "1"_e), "1"_e);
  // TODO implement basicReduction to make it works
  //auto [quotient, remainder] = Polynomial::PseudoDivision(polA, polB);
  //assert_trees_are_equal(quotient, Pol({1, 0}, "x"_e, Pol({1}, "y"_e, "1"_e), Sub("0"_e, "1"_e)));
  //assert_trees_are_equal(remainder, Pol({1, 0}, "x"_e, "1"_e, "1"_e));
}
