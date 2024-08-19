#include <apps/shared/global_context.h>
#include <poincare/src/layout/k_tree.h>
#include <poincare/src/layout/layout_cursor.h>

#include "helper.h"

using namespace Poincare;
using namespace Poincare::Internal::KTrees;

void assert_inserted_text_turns_into(const char* textToInsert,
                                     Layout expectedLayout,
                                     const char* textRightOfInsertedText = "") {
  Layout r = KRackL();
  Poincare::Internal::LayoutBufferCursor cursor(r, r.tree());
  Shared::GlobalContext context;
  cursor.insertText(textRightOfInsertedText, &context, false, true);
  size_t textLen = strlen(textToInsert);
  char buffer[2] = {0, 0};
  for (size_t i = 0; i < textLen; i++) {
    // Insert text char by char to apply beautification
    buffer[0] = textToInsert[i];
    cursor.insertText(buffer, &context);
  }
  quiz_assert(cursor.rootRack()->treeIsIdenticalTo(expectedLayout));
}

QUIZ_CASE(poincare_input_beautification_after_inserting_text) {
  // Beautify when input
  assert_inserted_text_turns_into("<= >= -> *", "≤ ≥ → ×"_l);

  // Beautify when followed by non identifier
  assert_inserted_text_turns_into("pi+theta+inf+", "π+θ+∞+"_l);

  // Parse identifiers implicit multiplication
  assert_inserted_text_turns_into("xpi+thetainf+", "xπ+θ∞+"_l);

  // Correctly beautify when parenthesed
  assert_inserted_text_turns_into(
      "pi))",
      KRackL(KParenthesesLeftTempL(KRackL(KParenthesesLeftTempL("π"_l)))));

  assert_inserted_text_turns_into(
      "(pi)+(theta)", KParenthesesL("π"_l) ^ "+"_l ^ KParenthesesL("θ"_l));

  // Do not alter normal behaviour
  assert_inserted_text_turns_into("((4))(",
                                  KParenthesesL(KRackL(KParenthesesL("4"_l))) ^
                                      KParenthesesRightTempL(KRackL()));

  // Correctly beautify pipe key
  assert_inserted_text_turns_into("1+|2+3", "1+"_l ^ KAbsL("2+3"_l));

  // 5+| inserted left of 6+7
  assert_inserted_text_turns_into("5+|", "5+"_l ^ KAbsL("6"_l) ^ "+7"_l, "6+7");

  // Beautify logN()
  assert_inserted_text_turns_into(
      "log52(6", "log"_l ^ KSubscriptL("52"_l) ^ KParenthesesL("6"_l));

  // Absorb arguments
  assert_inserted_text_turns_into("floor(", KRackL(KFloorL("4+6"_l)), "4+6");

  // Absorb multiple arguments
  assert_inserted_text_turns_into("root(", KRackL(KRootL("4"_l, "6"_l)), "4,6");

  // Do not beautify when too many arguments
  assert_inserted_text_turns_into(
      "floor(", "floor"_l ^ KParenthesesRightTempL("5,6"_l), "5,6");

  // Test all functions
  assert_inserted_text_turns_into("abs(", KRackL(KAbsL(KRackL())));

  assert_inserted_text_turns_into("binomial(",
                                  KRackL(KBinomialL(KRackL(), KRackL())));

  assert_inserted_text_turns_into("ceil(", KRackL(KCeilL(KRackL())));

  assert_inserted_text_turns_into("conj(", KRackL(KConjL(KRackL())));

  assert_inserted_text_turns_into(
      "diff(", KRackL(KDiffL("x"_l, KRackL(), "1"_l, KRackL())));

  assert_inserted_text_turns_into("exp(", "e"_l ^ KSuperscriptL(KRackL()));

  assert_inserted_text_turns_into("floor(", KRackL(KFloorL(KRackL())));

  assert_inserted_text_turns_into("norm(", KRackL(KVectorNormL(KRackL())));

  assert_inserted_text_turns_into("root(", KRackL(KRootL(KRackL(), KRackL())));

  assert_inserted_text_turns_into("sqrt(", KRackL(KSqrtL(KRackL())));

  assert_inserted_text_turns_into("piecewise(",
                                  KRackL(KPiecewise1L(KRackL(), KRackL())));

  assert_inserted_text_turns_into(
      "int(", KRackL(KIntegralL("x"_l, KRackL(), KRackL(), KRackL())));

  // Skip empty arguments
  assert_inserted_text_turns_into(
      "int(", KRackL(KIntegralL("x"_l, "4"_l, KRackL(), KRackL())), ",x,4");

  // Empty variable is replaced with default argument in parametered expression
  assert_inserted_text_turns_into(
      "int(", KRackL(KIntegralL("x"_l, "4"_l, KRackL(), "2"_l)), "2,,4");

  assert_inserted_text_turns_into("root(", KRackL(KRootL(KRackL(), "4"_l)),
                                  ",4");

  // Parse identifiers implicit multiplication
  assert_inserted_text_turns_into("pixsqrt(", "πx"_l ^ KSqrtL(KRackL()));

  // Sum is beautified only when inputing comma
  assert_inserted_text_turns_into("sum(",
                                  "sum"_l ^ KParenthesesRightTempL(KRackL()));

  assert_inserted_text_turns_into(
      "sum(3,", KRackL(KSumL("k"_l, KRackL(), KRackL(), "3"_l)));
}

#if 0
typedef void (LayoutCursor::*AddLayoutPointer)(Context* context);
typedef void (*CursorAddLayout)(LayoutCursor* cursor, Context* context,
                                AddLayoutPointer optionalAddLayoutFunction);

void assert_apply_beautification_after_layout_insertion(
    CursorAddLayout layoutInsertionFunction,
    AddLayoutPointer optionalAddLayoutFunction = nullptr) {
  HorizontalLayout horizontalLayout = HorizontalLayout::Builder();
  LayoutCursor cursor(horizontalLayout);
  Shared::GlobalContext context;
  cursor.insertText("pi", &context);
  (*layoutInsertionFunction)(&cursor, &context, optionalAddLayoutFunction);
  Layout piCodePoint = CodePointLayout::Builder(UCodePointGreekSmallLetterPi);
  if (optionalAddLayoutFunction ==
      &LayoutCursor::addFractionLayoutAndCollapseSiblings) {
    // Check numerator of created fraction
    quiz_assert(horizontalLayout.childAtIndex(0)
                    .childAtIndex(0)
                    .childAtIndex(0)
                    .isIdenticalTo(piCodePoint));
  } else {
    quiz_assert(horizontalLayout.childAtIndex(0).isIdenticalTo(piCodePoint));
  }
}
#endif

QUIZ_CASE(poincare_input_beautification_after_inserting_layout) {
#if 0
  AddLayoutPointer layoutInsertionFunction[] = {
      &LayoutCursor::addFractionLayoutAndCollapseSiblings,
      /* addEmptyExponentialLayout inserts 2 layouts so it's not beautified.
       * Not a problem until it is reported by users as being a problem .. */
      // &LayoutCursor::addEmptyExponentialLayout,
      &LayoutCursor::addEmptyPowerLayout,
      &LayoutCursor::addEmptySquareRootLayout,
      &LayoutCursor::addEmptySquarePowerLayout,
      &LayoutCursor::addEmptyTenPowerLayout,
      &LayoutCursor::addEmptyMatrixLayout};
  int numberOfFunctions = std::size(layoutInsertionFunction);

  for (int i = 0; i < numberOfFunctions; i++) {
    assert_apply_beautification_after_layout_insertion(
        [](LayoutCursor* cursor, Context* context,
           AddLayoutPointer optionalAddLayoutFunction) {
          (cursor->*optionalAddLayoutFunction)(context);
        },
        layoutInsertionFunction[i]);
  }
#endif
}

QUIZ_CASE(poincare_input_beautification_derivative) {
#if 0
  Shared::GlobalContext context;

  const HorizontalLayout firstOrderDerivative =
      HorizontalLayout::Builder(FirstOrderDerivativeLayout::Builder(
          HorizontalLayout::Builder(),
          HorizontalLayout::Builder(CodePointLayout::Builder('x')),
          HorizontalLayout::Builder()));
  const HorizontalLayout nthOrderDerivative =
      HorizontalLayout::Builder(HigherOrderDerivativeLayout::Builder(
          HorizontalLayout::Builder(),
          HorizontalLayout::Builder(CodePointLayout::Builder('x')),
          HorizontalLayout::Builder(), HorizontalLayout::Builder()));

  // Test d/dx()->diff()
  {
    HorizontalLayout h = HorizontalLayout::Builder(FractionLayout::Builder(
        HorizontalLayout::Builder(CodePointLayout::Builder('d')),
        HorizontalLayout::Builder(CodePointLayout::Builder('d'),
                                  CodePointLayout::Builder('x'))));
    LayoutCursor cursor(h);
    cursor.insertText("(", &context);
    quiz_assert(h.isIdenticalTo(firstOrderDerivative));
  }

  // Test d/dx^▯->d^▯/dx^▯
  {
    Layout h = firstOrderDerivative.clone();
    LayoutCursor cursor(h.childAtIndex(0).childAtIndex(1));
    cursor.addEmptyPowerLayout(&context);
    quiz_assert(h.isIdenticalTo(nthOrderDerivative));
  }

  // d/d|x -> "Power" -> d^▯/dx^▯
  {
    Layout h = firstOrderDerivative.clone();
    LayoutCursor cursor(h.childAtIndex(0).childAtIndex(1));
    cursor.addEmptyPowerLayout(&context);
    quiz_assert(h.isIdenticalTo(nthOrderDerivative));
  }
#endif
}
