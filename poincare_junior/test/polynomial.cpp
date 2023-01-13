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

void testPolynomialParsing() {
  assert_polynomial_is_parsed(
      /* π^3 + 3*π^2*e + 3*π*e^2 + e^3 */ Add(Pow(u'π'_n, "3"_n), Mult("3"_n, Pow(u'π'_n, "2"_n), u'e'_n), Mult("3"_n, Pow(u'e'_n, "2"_n), u'π'_n), Pow(u'e'_n, "3"_n)),
      /* variables = {π, e} */ Set(u'π'_n, u'e'_n),
      /* polynomial */ // TODO Pol(exponentsPi, u'π'_n, "1"_n, Pol(exponentsE0, u'e'_n, "3"_n), Pol(exponentsE1, u'e'_n, "3"_n), Pol(exponentsE2, u'e'_n, "1"_n))
      /* polynomial */ Pol({3, 2, 1, 0}, u'π'_n, "1"_n, Pol({1}, u'e'_n, Mult("3"_n, "1"_n, "1"_n)), Pol({2}, u'e'_n, Mult("3"_n, "1"_n, "1"_n)), Pol({3}, u'e'_n, "1"_n))
    );

  assert_polynomial_is_parsed(
      /* 42 */ "42"_n,
      /* variables = {} */ Set(),
      /* polynomial */ "42"_n
    );

  assert_polynomial_is_parsed(
      /* π^1.2 */ Pow(u'π'_n, 1.2_n),
      /* variables = {π^1.2} */ Set(Pow(u'π'_n, 1.2_n)),
      /* polynomial */ Pol({1}, Pow(u'π'_n, 1.2_n), "1"_n)
    );

  // TODO: parse polynomial with float coefficients?
}
QUIZ_CASE(pcj_polynomial_parsing) { testPolynomialParsing(); }

void testPolynomialOperations() {
  /* A = x^2 + 3*x*y + y + 1 */
  Node polA = Pol({2, 1, 0}, "x"_n, "1"_n, Pol({1},  "y"_n, "3"_n), Pol({1, 0}, "y"_n, "1"_n, "1"_n));
  /* B = x^3 + 2*x*y^2 + 7*x*y + 23 */
  Node polB = Pol({3, 1, 0}, "x"_n, "1"_n, Pol({2, 1},  "y"_n, "2"_n, "7"_n), "23"_n);

  /* A + B = x^3 + x^2 + 2*x*y^2 + 10*x*y + y + 24 */
  // TODO once basicReduction implemented assert_trees_are_equal(Polynomial::Addition(polA, polB), EditionReference(Pol({3, 2, 1, 0}, "x"_n, "1"_n, "1"_n, Pol({2, 1}, "y"_n, "2"_n, "10"_n), Pol({1, 0}, "y"_n, "1"_n, "24"_n))));
  assert_trees_are_equal(Polynomial::Addition(EditionReference(polA), EditionReference(polB)), EditionReference(Pol({3, 2, 1, 0}, "x"_n, "1"_n, "1"_n, Pol({2, 1}, "y"_n, "2"_n, Add("3"_n, "7"_n)), Pol({1, 0}, "y"_n, "1"_n, Add("1"_n, "23"_n)))));
  CachePool::sharedCachePool()->editionPool()->flush();

  /* A * B = x^5 + 3yx^4 + (2y^2+8y+1)*x^3 + (6y^3+21y^2+23)x^2 + (2y^3+9y^2+ 76y)x + 23y + 23 */
  // TODO once basicReduction assert_trees_are_equal(Polynomial::Multiplication(EditionReference(polA), EditionReference(polB)), EditionReference(Pol({5, 4, 3, 2, 1, 0}, "x"_n, "1"_n, Pol({1}, "y"_n, "3"_n), Pol({2, 1, 0}, "y"_n, "2"_n, "8"_n, "1"_n), Pol({3, 2, 0}, "y"_n, "6"_n, "21"_n, "23"_n), Pol({3, 2, 1}, "y"_n, "2"_n, "9"_n, "76"_n, Pol({1, 0}, "y"_n, "23"_n, "23"_n))));
  assert_trees_are_equal(Polynomial::Multiplication(EditionReference(polA), EditionReference(polB)), EditionReference(Pol({5, 4, 3, 2, 1, 0}, "x"_n, Mult("1"_n, "1"_n), Pol({1}, "y"_n, Mult("3"_n, "1"_n)), Pol({2, 1, 0}, "y"_n, Mult("2"_n, "1"_n), Add( Mult("1"_n, "1"_n), Mult("7"_n, "1"_n)), Mult("1"_n, "1"_n)), Pol({3, 2, 0}, "y"_n,  Mult("3"_n, "2"_n), Mult("3"_n, "7"_n), Mult("1"_n, "23"_n)), Pol({3, 2, 1}, "y"_n, Mult("1"_n, "2"_n), Add(Mult("1"_n, "2"_n), Mult("1"_n, "7"_n)), Add(Mult("1"_n, "7"_n), Mult("3"_n, "23"_n))), Pol({1, 0}, "y"_n, Mult("1"_n, "23"_n), Mult("1"_n, "23"_n)))));
  CachePool::sharedCachePool()->editionPool()->flush();

  /* Test variable order:
   * (y^2) + ((y+1)x + 1 = (y+1)x + y^2 + 1 */
  assert_trees_are_equal(Polynomial::Addition(EditionReference(Pol({2}, "y"_n, "1"_n)), EditionReference(Pol({1,0}, "x"_n, Pol({1,0}, "y"_n, "1"_n, "1"_n), "1"_n))), Pol({1, 0}, "x"_n, Pol({1,0}, "y"_n, "1"_n, "1"_n), Pol({2,0}, "y"_n, "1"_n, "1"_n)));
  CachePool::sharedCachePool()->editionPool()->flush();

  // A = x^2y^2 + x
  polA = Pol({2,1}, "x"_n, Pol({2}, "y"_n, "1"_n), "1"_n);
  // B = xy + 1
  polB = Pol({1,0}, "x"_n, Pol({1}, "y"_n, "1"_n), "1"_n);
  // TODO implement basicReduction to make it works
  //auto [quotient, remainder] = Polynomial::PseudoDivision(polA, polB);
  //assert_trees_are_equal(quotient, Pol({1, 0}, "x"_n, Pol({1}, "y"_n, "1"_n), Sub("0"_n, "1"_n)));
  //assert_trees_are_equal(remainder, Pol({1, 0}, "x"_n, "1"_n, "1"_n));
}
QUIZ_CASE(pcj_polynomial_operations) { testPolynomialOperations(); }
