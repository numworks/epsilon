#include "print.h"
#include <poincare_junior/src/expression/polynomial.h>
#include <poincare_junior/src/memory/tree_constructor.h>

using namespace Poincare;

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
      /* π^3 + 3*π^2*e + 3*π*e^2 + e^3 */ Add(Pow(u'π'_n, 3_n), Mult(3_n, Pow(u'π'_n, 2_sn), u'e'_n), Mult(3_n, Pow(u'e'_n, 2_sn), u'π'_n), Pow(u'e'_n, 3_n)),
      /* variables = {π, e} */ Set(u'π'_n, u'e'_n),
      /* polynomial */ // TODO Pol(exponentsPi, u'π'_n, 1_sn, Pol(exponentsE0, u'e'_n, 3_n), Pol(exponentsE1, u'e'_n, 3_n), Pol(exponentsE2, u'e'_n, 1_sn))
      /* polynomial */ Pol({3, 2, 1, 0}, u'π'_n, 1_sn, Pol({1}, u'e'_n, Mult(3_n, 1_sn, 1_sn)), Pol({2}, u'e'_n, Mult(3_n, 1_sn, 1_sn)), Pol({3}, u'e'_n, 1_sn))
    );

  assert_polynomial_is_parsed(
      /* 42 */ 42_n,
      /* variables = {} */ Set(),
      /* polynomial */ 42_n
    );

  assert_polynomial_is_parsed(
      /* π^1.2 */ Pow(u'π'_n, 1.2_fn),
      /* variables = {π^1.2} */ Set(Pow(u'π'_n, 1.2_fn)),
      /* polynomial */ Pol({1}, Pow(u'π'_n, 1.2_fn), 1_sn)
    );

  // TODO: parse polynomial with float coefficients?
}

void testPolynomialOperations() {
  /* A = x^2 + 3*x*y + y + 1 */
  Node polA = Pol({2, 1, 0}, Symb("x"), 1_sn, Pol({1},  Symb("y"), 3_n), Pol({1, 0}, Symb("y"), 1_sn, 1_sn));
  /* B = x^3 + 2*x*y^2 + 7*x*y + 23 */
  Node polB = Pol({3, 1, 0}, Symb("x"), 1_sn, Pol({2, 1},  Symb("y"), 2_sn, 7_n), 23_n);

  /* A + B = x^3 + x^2 + 2*x*y^2 + 10*x*y + y + 24 */
  // TODO once basicReduction implemented assert_trees_are_equal(Polynomial::Addition(polA, polB), EditionReference(Pol({3, 2, 1, 0}, Symb("x"), 1_sn, 1_sn, Pol({2, 1}, Symb("y"), 2_sn, 10_n), Pol({1, 0}, Symb("y"), 1_sn, 24_n))));
  assert_trees_are_equal(Polynomial::Addition(EditionReference(polA), EditionReference(polB)), EditionReference(Pol({3, 2, 1, 0}, Symb("x"), 1_sn, 1_sn, Pol({2, 1}, Symb("y"), 2_sn, Add(3_n, 7_n)), Pol({1, 0}, Symb("y"), 1_sn, Add(1_sn, 23_n)))));
  CachePool::sharedCachePool()->editionPool()->flush();

  /* A * B = x^5 + 3yx^4 + (2y^2+8y+1)*x^3 + (6y^3+21y^2+23)x^2 + (2y^3+9y^2+ 76y)x + 23y + 23 */
  // TODO once basicReduction assert_trees_are_equal(Polynomial::Multiplication(EditionReference(polA), EditionReference(polB)), EditionReference(Pol({5, 4, 3, 2, 1, 0}, Symb("x"), 1_sn, Pol({1}, Symb("y"), 3_n), Pol({2, 1, 0}, Symb("y"), 2_sn, 8_n, 1_sn), Pol({3, 2, 0}, Symb("y"), 6_n, 21_n, 23_n), Pol({3, 2, 1}, Symb("y"), 2_sn, 9_n, 76_n, Pol({1, 0}, Symb("y"), 23_n, 23_n))));
  assert_trees_are_equal(Polynomial::Multiplication(EditionReference(polA), EditionReference(polB)), EditionReference(Pol({5, 4, 3, 2, 1, 0}, Symb("x"), Mult(1_sn, 1_sn), Pol({1}, Symb("y"), Mult(3_n, 1_sn)), Pol({2, 1, 0}, Symb("y"), Mult(2_sn, 1_sn), Add( Mult(1_sn, 1_sn), Mult(7_n, 1_sn)), Mult(1_sn, 1_sn)), Pol({3, 2, 0}, Symb("y"),  Mult(3_n, 2_sn), Mult(3_n, 7_n), Mult(1_sn, 23_n)), Pol({3, 2, 1}, Symb("y"), Mult(1_sn, 2_sn), Add(Mult(1_sn, 2_sn), Mult(1_sn, 7_n)), Add(Mult(1_sn, 7_n), Mult(3_n, 23_n))), Pol({1, 0}, Symb("y"), Mult(1_sn, 23_n), Mult(1_sn, 23_n)))));
  CachePool::sharedCachePool()->editionPool()->flush();

  /* */
}
