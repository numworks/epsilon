#include <ion/display.h>
#include <omg/unicode_helper.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/layout/grid.h>
#include <poincare/src/layout/k_tree.h>
#include <poincare/src/layout/layouter.h>
#include <poincare/src/layout/multiplication_symbol.h>
#include <poincare/src/layout/render.h>

#include "helper.h"

using namespace Poincare::Internal;

QUIZ_CASE(pcj_expression_to_layout) {
  assert_trees_are_equal(
      Layouter::LayoutExpression(KPow(KAdd("x"_e, "y"_e), 2_e)->cloneTree()),
      KRackL(KParenthesesL("x+y"_l), KSuperscriptL("2"_l)));
  assert_trees_are_equal(
      Layouter::LayoutExpression(KPow(KMult("x"_e, "y"_e), 2_e)->cloneTree()),
      KRackL(KParenthesesL("x·y"_l), KSuperscriptL("2"_l)));
  assert_trees_are_equal(
      Layouter::LayoutExpression(
          KAdd(KMixedFraction(2_e, KDiv(1_e, 3_e)), 4_e)->cloneTree(), true),
      "2 1/3+4"_l);
  assert_trees_are_equal(
      Layouter::LayoutExpression(KAdd(12345_e, KOpposite(54321_e))->cloneTree(),
                                 false),
      // 12 345 - 54 321
      "12"_l ^ KThousandSeparatorL ^ "345"_l ^ KOperatorSeparatorL ^ "-"_l ^
          KOperatorSeparatorL ^ "54"_l ^ KThousandSeparatorL ^ "321"_l);
  assert_trees_are_equal(
      Layouter::LayoutExpression(KAdd(12345_de, -54321_de)->cloneTree(), false),
      "12"_l ^ KThousandSeparatorL ^ "345"_l ^ KOperatorSeparatorL ^ "-"_l ^
          KOperatorSeparatorL ^ "54"_l ^ KThousandSeparatorL ^ "321"_l);
}

QUIZ_CASE(pcj_layout_decoder) {
  const Tree* l = "123"_l;
  CPLayoutDecoder d(l->child(0), 0, 3);
  quiz_assert(d.nextCodePoint() == '1');
  quiz_assert(d.nextCodePoint() == '2');
  quiz_assert(d.nextCodePoint() == '3');
  quiz_assert(d.nextCodePoint() == 0);
}

QUIZ_CASE(pcj_omg_code_point) {
  const LayoutSpan l(Rack::From("123"_l));
  quiz_assert(CodePointSearch(l, '2') == 1);
  quiz_assert(CodePointSearch(l, '4') == l.length);
}

#if 0
QUIZ _CASE(pcj_layout_render) {
  KDContext* ctx = Ion::Display::Context::SharedContext;
  // TODO layoutCursor is nullptr and expected
  Layout l = Layout(
      KRackL("1+"_l,
             KParenthesesL(KRackL(
                 "2×"_l, KParenthesesL(KRackL("1+"_l, KFracL("1"_l, "2"_l))))),
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
