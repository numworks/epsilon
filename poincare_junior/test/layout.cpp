#include "print.h"
#include <poincare_junior/include/expression.h>
#include <poincare_junior/include/layout.h>

using namespace PoincareJ;

void assert_equal(const Layout l1, const Layout l2) {
  constexpr static int k_bufferSize = 128;
  TypeBlock t1[k_bufferSize];
  TypeBlock t2[k_bufferSize];
  l2.dumpAt(t2);
  l1.dumpAt(t1);
  assert(Node(t1).isIdenticalTo(Node(t2)));
}

void testLayoutCreation() {
  Layout l1 = Layout::CreateLayoutFromText("-1+2*3");
  Expression e1 = Expression::CreateExpressionFromText("-1+2*3");
  Layout l2 = Layout::CreateLayoutFromExpression(&e1);
  assert_equal(l1, l2);
}

QUIZ_CASE(pcj_layout_creation) { testLayoutCreation(); }
