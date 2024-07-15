#include <poincare/api.h>
#include <poincare/k_tree.h>
#include <quiz.h>

using namespace Poincare::API;

QUIZ_CASE(pcj_api) {
  UserExpression ue = UserExpression::Builder(KDiv(1_e, "x"_e));
  quiz_assert(ue.tree()->treeIsIdenticalTo(KDiv(1_e, "x"_e)));
  // TODO_PCJ: this crashes on device tests, investigate
  SystemExpression se = ue.projected();
  quiz_assert(se.tree()->treeIsIdenticalTo(KPow("x"_e, -1_e)));
  UserExpression ue2 = se.beautified();
  quiz_assert(ue2.tree()->treeIsIdenticalTo(KDiv(1_e, "x"_e)));
  Layout l = ue2.createLayout();
  quiz_assert(l.tree()->treeIsIdenticalTo(KRackL(KFracL("1"_l, "x"_l))));
}
