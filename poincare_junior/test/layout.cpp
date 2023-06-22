#include <kandinsky/ion_context.h>
#include <poincare_junior/include/expression.h>
#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/expression/k_creator.h>
#include <poincare_junior/src/layout/k_creator.h>
#include <poincare_junior/src/layout/render.h>

#include "helper.h"

using namespace PoincareJ;

QUIZ_CASE(pcj_layout_creation) {
  // text -> Layout
  Layout l1 = Layout::Parse("1+2*3");
  // text -> Expression -> Layout
  Expression e1 = Expression::Parse("1+2*3");
  Layout l2 = e1.toLayout();
  assert(l2.treeIsIdenticalTo(l1));
  // expression node -> Expression -> Layout
  Expression e2 = Expression(KAdd(1_e, KMult(2_e, 3_e)));
  Layout l3 = e2.toLayout();
  // layout Node -> Layout
  assert(l3.treeIsIdenticalTo(l1));
  // constexpr tree -> Layout
  Layout l4 = Layout("1+2*3"_l);
  assert(l4.treeIsIdenticalTo(l1));
}

QUIZ_CASE(pcj_layout_render) {
  KDContext* ctx = KDIonContext::SharedContext;
  Layout l = Layout(
      KRackL("1+"_l,
             KParenthesisL(KRackL(
                 "2*"_l, KParenthesisL(KRackL("1+"_l, KFracL("1"_l, "2"_l))))),
             KVertOffL("2"_l), "-2"_l));
  l.draw(ctx, KDPoint(10, 100), KDFont::Size::Large);
}
