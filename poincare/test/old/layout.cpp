#include <poincare/layout.h>
#include <poincare/src/layout/k_tree.h>
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
  Layout e28 = Layout::Create(KRackL(KDiffL(KA, KB, KC, KD)),
                              {.KA = e15, .KB = e27, .KC = e0, .KD = e26});
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
