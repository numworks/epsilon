#include "print.h"
#include <poincare_junior/src/expression/polynomial.h>
#include <poincare_junior/src/memory/tree_constructor.h>

using namespace Poincare;

void assert_polynomial_is_parsed(const Node node, const Node expectedVariables, const Node expectedPolynomial) {
  EditionReference variables = PolynomialParser::GetVariables(node);
  assert_trees_are_equal(variables.node(), expectedVariables);
  EditionReference ref(node);
  EditionReference polynomial = PolynomialParser::RecursivelyParse(ref, variables.node());
  assert_trees_are_equal(polynomial, expectedPolynomial);
}

void testPolynomialParsing() {
  uint8_t exponentsPi[] = {3, 2, 1, 0};
  uint8_t exponentsE0[] = {1};
  uint8_t exponentsE1[] = {2};
  uint8_t exponentsE2[] = {3};
  assert_polynomial_is_parsed(
      /* π^3 + 3*π^2*e + 3*π*e^2 + e^3 */ Add(Pow(u'π'_n, 3_n), Mult(3_n, Pow(u'π'_n, 2_sn), u'e'_n), Mult(3_n, Pow(u'e'_n, 2_sn), u'π'_n), Pow(u'e'_n, 3_n)),
      /* variables = {π, e} */ Set(u'π'_n, u'e'_n),
      /* polynomial */ // TODO Pol(exponentsPi, 1_sn, Pol(exponentsE0, 3_n), Pol(exponentsE1, 3_n), Pol(exponentsE2, 1_sn))
      /* polynomial */ Pol(exponentsPi, 1_sn, Pol(exponentsE0, Mult(3_n, 1_sn, 1_sn)), Pol(exponentsE1, Mult(3_n, 1_sn, 1_sn)), Pol(exponentsE2, 1_sn))
    );
#if 0
  assert_polynomial_is_parsed(
      /* 42 */ 42_n,
      /* variables = {} */ Set(),
      /* degree = 0 */ {}
    );
  assert_polynomial_is_parsed(
      /* π^1.2 */ Pow(u'π'_n, 1.2_fn),
      /* variables = {π^1.2} */ Set(Pow(u'π'_n, 1.2_fn)),
      /* degree = 1 */ {1}
    );
#endif
}
