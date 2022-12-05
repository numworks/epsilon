#include "print.h"
#include <poincare_junior/src/expression/polynomial.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/memory/tree_constructor.h>

using namespace Poincare;

void assert_get_variable_equals(const Node node, const Node result) {
  EditionReference variables = Polynomial::GetVariables(node);
  assert(Simplification::Compare(variables.node(), result) == 0);
}

void testVariables() {
  // Polynomial::GetVariables
  // π^3 +3*π^2*e+3*π*e^2 +e^3
  assert_get_variable_equals(Add(Pow(u'π'_n, 3_n), Mult(3_n, Pow(u'π'_n, 2_sn), u'e'_n), Mult(3_n, Pow(u'e'_n, 2_sn), u'π'_n), Pow(u'e'_n, 3_n)), Set(u'π'_n, u'e'_n));
  assert_get_variable_equals(42_n, Set());
  assert_get_variable_equals(Pow(u'π'_n, 1.2_fn), Set(Pow(u'π'_n, 1.2_fn)));

  print();
}

