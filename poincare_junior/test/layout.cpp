#include "print.h"
#include <poincare_junior/include/expression.h>
#include <poincare_junior/include/layout.h>

using namespace PoincareJ;

QUIZ_CASE(pcj_layout_creation) {
  Layout l1 = Layout::Parse("-1+2*3");
  Expression e1 = Expression::Parse("-1+2*3");
  Layout l2 = e1.toLayout();
  assert(l1.treeIsIdenticalTo(l2));
}
