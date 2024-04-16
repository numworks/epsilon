#include <kandinsky/ion_context.h>
#include <omg/unicode_helper.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/layout/grid.h>
#include <poincare/src/layout/k_tree.h>
#include <poincare/src/layout/layoutter.h>
#include <poincare/src/layout/multiplication_symbol.h>
#include <poincare/src/layout/render.h>

#include "helper.h"

using namespace Poincare::Internal;

QUIZ_CASE(pcj_expression_to_layout) {
  assert_trees_are_equal(
      Layoutter::LayoutExpression(KPow(KAdd("x"_e, "y"_e), 2_e)->clone()),
      KRackL(KParenthesisL("x+y"_l), KSuperscriptL("2"_l)));
  assert_trees_are_equal(
      Layoutter::LayoutExpression(KPow(KMult("x"_e, "y"_e), 2_e)->clone()),
      KRackL(KParenthesisL("x·y"_l), KSuperscriptL("2"_l)));
  assert_trees_are_equal(
      Layoutter::LayoutExpression(
          KAdd(KMixedFraction(2_e, 1_e / 3_e), 4_e)->clone(), true),
      "2 1/3+4"_l);
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
             /*KSuperscriptL("2"_l),*/ "-2"_l));
  l.draw(ctx, KDPoint(10, 100), KDFont::Size::Large);
}
#endif

QUIZ_CASE(pcj_layout_multiplication_symbol) {
  quiz_assert(MultiplicationSymbol(KMult(2_e, 3_e)) == u'×');
  quiz_assert(MultiplicationSymbol(KMult(2_e, "a"_e)) == UCodePointNull);
  quiz_assert(MultiplicationSymbol(KMult(2_e, KCos(π_e), KSqrt(2_e))) == u'·');
}

QUIZ_CASE(pcj_k_matrix_l) {
  const Grid* mat = Grid::From(KMatrix1x1L("a"_l));
  quiz_assert(mat->numberOfColumns() == 2);
  quiz_assert(mat->numberOfRows() == 2);
  quiz_assert(mat->childAt(0, 0)->treeIsIdenticalTo("a"_l));

  mat = Grid::From(KMatrix2x2L("a"_l, "b"_l, "c"_l, "d"_l));
  quiz_assert(mat->numberOfColumns() == 3);
  quiz_assert(mat->numberOfRows() == 3);
  quiz_assert(mat->childAt(1, 1)->treeIsIdenticalTo("d"_l));
}
