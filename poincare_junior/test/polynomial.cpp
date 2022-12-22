#include "print.h"
#include <poincare_junior/src/expression/polynomial.h>
#include <poincare_junior/src/memory/tree_constructor.h>

using namespace Poincare;

void assert_polynomial_is_parsed(const Node node, const Node expectedVariables, uint8_t expectedDegree) {
  EditionReference variables = Polynomial::GetVariables(node);
  uint8_t degree = Polynomial::Degree(node, variables.node());
  assert_trees_are_equal(variables.node(), expectedVariables);
  assert(degree == expectedDegree);
}

void testPolynomialParsing() {
  assert_polynomial_is_parsed(
      /* π^3 +3*π^2*e+3*π*e^2 +e^3 */ Add(Pow(u'π'_n, 3_n), Mult(3_n, Pow(u'π'_n, 2_sn), u'e'_n), Mult(3_n, Pow(u'e'_n, 2_sn), u'π'_n), Pow(u'e'_n, 3_n)),
      /* variables = {π, e} */ Set(u'π'_n, u'e'_n),
      /* degree = 3 */ 3
    );
  assert_polynomial_is_parsed(
      /* 42 */ 42_n,
      /* variables = {} */ Set(),
      /* degree = 0 */ 0
    );
  assert_polynomial_is_parsed(
      /* π^1.2 */ Pow(u'π'_n, 1.2_fn),
      /* variables = {π^1.2} */ Set(Pow(u'π'_n, 1.2_fn)),
      /* degree = 1 */ 1
    );
}
