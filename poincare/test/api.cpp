#include <poincare/api.h>
#include <poincare/k_tree.h>

#include "helper.h"

using namespace Poincare;

QUIZ_CASE(pcj_api) {
  UserExpression ue = UserExpression::Builder(KDiv(1_e, "x"_e));
  ue.tree()->log();
  SystemExpression se = ue.projected();
  se.tree()->log();
  UserExpression ue2 = se.beautified();
  ue2.tree()->log();
}
