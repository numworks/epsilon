#include <poincare/api.h>
#include <poincare/k_tree.h>
#include <quiz.h>

using namespace Poincare::API;

QUIZ_CASE(pcj_api) {
  UserExpression ue = UserExpression::Builder(KDiv(1_e, "x"_e));
  ue.tree()->log();
  SystemExpression se = ue.projected();
  se.tree()->log();
  UserExpression ue2 = se.beautified();
  ue2.tree()->log();
  Layout l = ue2.createLayout();
  l.tree()->log();
}
