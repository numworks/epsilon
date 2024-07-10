#include <poincare/src/layout/k_tree.h>
#include <poincare/src/layout/layout_cursor.h>
#include <poincare/src/memory/pattern_matching.h>

#include "helper.h"

using namespace Poincare;
using namespace Poincare::Internal::KTrees;

QUIZ_CASE(poincare_layout_constructors) {
  Layout e0 = KRackL();
  Layout e1 = Layout::Create(KRackL(KAbsL(KA)), {.KA = e0});
  Layout e2 = "a"_l;
  Layout e3 = Layout::Create(KRackL(KBinomialL(KA, KB)), {.KA = e1, .KB = e2});
  Layout e4 = Layout::Create(KRackL(KCeilL(KA)), {.KA = e3});
  Layout e5 = KRackL(KParenthesesL(KRackL()));
  Layout e7 = Layout::Create(KRackL(KCondensedSumL(KA, KB, KC)),
                             {.KA = e4, .KB = e5, .KC = e3});
  Layout e8 = Layout::Create(KRackL(KConjL(KA)), {.KA = e7});
  Layout e10 = KRackL(KCurlyBracesL(KRackL()));
  Layout e11 = Layout::Create(KRackL(KFloorL(KA)), {.KA = e10});
  Layout e12 =
      Layout::Create(KRackL(KBinomialL(KA, KB)), {.KA = e8, .KB = e11});
  Layout e13 = KRackL();
  Layout e15 = Layout::Create(KRackL(KIntegralL(KA, KB, KC, KD)),
                              {.KA = e11, .KB = e12, .KC = e13, .KD = e10});
  Layout e16 = Layout::Create(KRackL(KSqrtL(KA)), {.KA = e15});
  Layout e17 = KRackL(KEmptyMatrixL);
  Layout e18 = KRackL();
  Layout e19 = KRackL();
  Layout e20 = KRackL();
  Layout e21 = Layout::Create(KRackL(KProductL(KA, KB, KC, KD)),
                              {.KA = e17, .KB = e18, .KC = e19, .KD = e20});
  Layout e22 = KRackL();
  Layout e23 = KRackL();
  Layout e24 = KRackL();
  Layout e25 = Layout::Create(KRackL(KSumL(KA, KB, KC, KD)),
                              {.KA = e21, .KB = e22, .KC = e23, .KD = e24});
  Layout e26 = Layout::Create(KRackL(KSuperscriptL(KA)), {.KA = e25});
  Layout e27 = "t"_l;
  Layout e28 = Layout::Create(KRackL(KDiffL(KA, KB, KC)),
                              {.KA = e15, .KB = e27, .KC = e26});
  Layout e29 = Layout::Create(KRackL(KNthDiffL(KA, KB, KC, KD)),
                              {.KA = e15, .KB = e27, .KC = e26, .KD = e21});
  Layout e30 = "Hé"_l;
  // TODO: Layout e31 = KPiecewiseL();
}

QUIZ_CASE(poincare_layout_comparison) {
  Layout e0 = "a"_l;
  Layout e1 = "a"_l;
  Layout e2 = "b"_l;
  quiz_assert(e0.isIdenticalTo(e1));
  quiz_assert(!e0.isIdenticalTo(e2));

  Layout e3 = KRackL();
  Layout e4 = KRackL();
  quiz_assert(e3.isIdenticalTo(e4));
  quiz_assert(!e3.isIdenticalTo(e0));

  Layout e5 = Layout::Create(KRackL(KSqrtL(KA)), {.KA = e0});
  Layout e6 = Layout::Create(KRackL(KSqrtL(KA)), {.KA = e1});
  Layout e7 = Layout::Create(KRackL(KSqrtL(KA)), {.KA = e2});
  quiz_assert(e5.isIdenticalTo(e6));
  quiz_assert(!e5.isIdenticalTo(e7));
  quiz_assert(!e5.isIdenticalTo(e0));

  Layout e8 = Layout::Create(KRackL(KSuperscriptL(KA)), {.KA = e5});
  Layout e9 = Layout::Create(KRackL(KSuperscriptL(KA)), {.KA = e6});
  Layout e10 = KRackL(KSubscriptL(KRackL(KSqrtL("a"_l))));
  quiz_assert(e8.isIdenticalTo(e9));
  quiz_assert(!e8.isIdenticalTo(e10));
  quiz_assert(!e8.isIdenticalTo(e0));

  Layout e11 = Layout::Create(KRackL(KSumL(KA, KB, KC, KD)),
                              {.KA = e0, .KB = e3, .KC = e6, .KD = e2});
  Layout e12 = KRackL(KSumL("a"_l, KRackL(), KRackL(KSqrtL("a"_l)), "b"_l));
  Layout e13 = KRackL(KProductL("a"_l, KRackL(), KRackL(KSqrtL("a"_l)), "b"_l));
  quiz_assert(e11.isIdenticalTo(e12));
  quiz_assert(!e11.isIdenticalTo(e13));
}

QUIZ_CASE(poincare_layout_fraction_create) {
  /*                         12
   * 12|34+5 -> "Divide" -> --- + 5
   *                        |34
   * */
  Layout l1 = "1234+5"_l;
  Poincare::Internal::LayoutBufferCursor c1(l1, l1.tree());
  c1.safeSetPosition(2);
  c1.addFractionLayoutAndCollapseSiblings(nullptr);
  quiz_assert(
      c1.rootNode()->treeIsIdenticalTo(KFracL("12"_l, "34"_l) ^ "+5"_l));
  quiz_assert(c1.cursorNode() == c1.rootNode()->child(0)->child(1) &&
              c1.position() == 0);

  /*                     |
   * |34+5 -> "Divide" -> --- + 5
   *                      34
   * */
  Layout l2 = "34+5"_l;
  Poincare::Internal::LayoutBufferCursor c2(l2, l2.tree(),
                                            OMG::Direction::Left());
  c2.addFractionLayoutAndCollapseSiblings(nullptr);
  quiz_assert(c2.rootNode()->treeIsIdenticalTo(KFracL(""_l, "34"_l) ^ "+5"_l));
  quiz_assert(c2.cursorNode() == c2.rootNode()->child(0)->child(0) &&
              c2.position() == 0);

  /*
   *  1                      1   3
   * --- 3|4 -> "Divide" -> --- ---
   *  2                      2  |4
   * */
  Layout l3 = KFracL("1"_l, "2"_l) ^ "34"_l;
  Poincare::Internal::LayoutBufferCursor c3(l3, l3.tree());
  c3.safeSetPosition(2);
  c3.addFractionLayoutAndCollapseSiblings(nullptr);
  quiz_assert(c3.rootNode()->treeIsIdenticalTo(KFracL("1"_l, "2"_l) ^
                                               KFracL("3"_l, "4"_l)));
  quiz_assert(c3.cursorNode() == c3.rootNode()->child(1)->child(1) &&
              c3.position() == 0);

  /*
   *                                sin(x)cos(x)
   * sin(x)cos(x)|2 -> "Divide" -> --------------
   *                                    |2
   * */
  Layout l4 = "sin(x)cos(x)2"_l;
  Poincare::Internal::LayoutBufferCursor c4(l4, l4.tree());
  c4.safeSetPosition(l4.numberOfChildren() - 1);
#if 0  // TODO_PCJ: fails
  c4.addFractionLayoutAndCollapseSiblings(nullptr);
  quiz_assert(c4.rootNode()->treeIsIdenticalTo(KRackL(KFracL("sin(x)cos(x)"_l, "2"_l)));
  quiz_assert(c4.cursorNode() == c4.rootNode()->child(0)->child(1) &&
              c4.position() == 0);
#endif
}

QUIZ_CASE(poincare_layout_power) {
  /*
   *                      2|
   * 12| -> "Square" -> 12 |
   *
   * */
  Layout l1 = "12"_l;
  Poincare::Internal::LayoutBufferCursor c1(l1, l1.tree());
  c1.addEmptySquarePowerLayout(nullptr);
  quiz_assert(c1.rootNode()->treeIsIdenticalTo("12"_l ^ KSuperscriptL("2"_l)));
  quiz_assert(c1.cursorNode() == c1.rootNode() &&
              c1.position() == c1.rootNode()->numberOfChildren());

  /*                        2|
   *  2|                ( 2) |
   * 1 | -> "Square" -> (1 ) |
   *
   * */
  Layout l2 = KRackL("1"_cl, KSuperscriptL("2"_l));
  Poincare::Internal::LayoutBufferCursor c2(l2, l2.tree()->child(1));
  c2.addEmptySquarePowerLayout(nullptr);
  quiz_assert(c2.rootNode()->treeIsIdenticalTo(
      KParenthesesL("1"_l ^ KSuperscriptL("2"_l)) ^ KSuperscriptL("2"_l)));
  quiz_assert(c2.cursorNode() == c2.rootNode() &&
              c2.position() == c2.rootNode()->numberOfChildren());

  /*                             (    2|)
   * ( 2)|                       (( 2) |)
   * (1 )| -> "Left" "Square" -> ((1 ) |)
   * */
  Layout l3 = KRackL(KParenthesesL(KRackL("1"_cl, KSuperscriptL("2"_l))));
  Poincare::Internal::LayoutBufferCursor c3(l3, l3.tree());
  bool dummy;
  c3.move(OMG::Direction::Left(), false, &dummy);
  c3.addEmptySquarePowerLayout(nullptr);
  quiz_assert(c3.rootNode()->treeIsIdenticalTo(KRackL(KParenthesesL(
      KParenthesesL("1"_l ^ KSuperscriptL("2"_l)) ^ KSuperscriptL("2"_l)))));
  quiz_assert(c3.cursorNode() == c3.rootNode()->child(0)->child(0) &&
              c3.position() == c3.cursorNode()->numberOfChildren());
}
