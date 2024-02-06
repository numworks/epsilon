#include <kandinsky/ion_context.h>
#include <omgpj/unicode_helper.h>
#include <poincare_junior/include/expression.h>
#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/layout/k_tree.h>
#include <poincare_junior/src/layout/layoutter.h>
#include <poincare_junior/src/layout/render.h>

#include "helper.h"

using namespace PoincareJ;

QUIZ_CASE(pcj_layout_creation) {
  // text -> Layout
  Layout l1 = Layout::Parse("1+2×3");
  // text -> Expression -> Layout
  Expression e1 = Expression::Parse("1+2×3");
  Layout l2 = Layout::FromExpression(&e1);
  assert(l2.treeIsIdenticalTo(l1));
  // expression node -> Expression -> Layout
  Expression e2 = Expression(KAdd(1_e, KMult(2_e, 3_e)));
  Layout l3 = Layout::FromExpression(&e2);
  // layout Tree -> Layout
  assert(l3.treeIsIdenticalTo(l1));
  // constexpr tree -> Layout
  Layout l4 = Layout("1+2×3"_l);
  assert(l4.treeIsIdenticalTo(l1));
}

QUIZ_CASE(pcj_expression_to_layout) {
  assert_trees_are_equal(
      Layoutter::LayoutExpression(KPow(KAdd("x"_e, "y"_e), 2_e)->clone()),
      KRackL(KParenthesisL("x+y"_l), KVertOffL("2"_l)));
  assert_trees_are_equal(
      Layoutter::LayoutExpression(KPow(KMult("x"_e, "y"_e), 2_e)->clone()),
      KRackL(KParenthesisL("x×y"_l), KVertOffL("2"_l)));
}

QUIZ_CASE(pcj_layout_decoder) {
  const Tree* l = "123"_l;
  CPLayoutDecoder d(l->child(0), 0, 3);
  assert(d.nextCodePoint() == '1');
  assert(d.nextCodePoint() == '2');
  assert(d.nextCodePoint() == '3');
  assert(d.nextCodePoint() == 0);
}

QUIZ_CASE(pcj_omg_code_point) {
  const CPL* l = CPL::FromRack("123"_l);
  assert(OMG::CodePointLSearch(l, '2', l + 3) == l + 1);
  assert(OMG::CodePointLSearch(l, '4', l + 3) == l + 3);
}

#if 0
QUIZ _CASE(pcj_layout_render) {
  KDContext* ctx = KDIonContext::SharedContext;
  // TODO layoutCursor is nullptr and expected
  Layout l = Layout(
      KRackL("1+"_l,
             KParenthesisL(KRackL(
                 "2×"_l, KParenthesisL(KRackL("1+"_l, KFracL("1"_l, "2"_l))))),
             /*KVertOffL("2"_l),*/ "-2"_l));
  l.draw(ctx, KDPoint(10, 100), KDFont::Size::Large);
}
#endif
