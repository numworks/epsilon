#include "print.h"
#include <poincare_junior/include/expression.h>
#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/memory/tree_constructor.h>

using namespace PoincareJ;

QUIZ_CASE(pcj_layout_creation) {
  // text -> Layout
  Layout l1 = Layout::Parse("-1+2*3");
  // text -> Expression -> Layout
  Expression e1 = Expression::Parse("-1+2*3");
  Layout l2 = e1.toLayout();
  // expression node -> Expression -> Layout
  Expression e2 = Expression(Add("-1"_e, Mult("2"_e, "3"_e)));
  Layout l3 = e2.toLayout();
  // layout Node -> Layout
  Layout l4 = Layout("-1+2*3"_l);
  assert(l2.treeIsIdenticalTo(l1));
  assert(l3.treeIsIdenticalTo(l1));
  assert(l4.treeIsIdenticalTo(l1));
}
