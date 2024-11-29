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
  Poincare::Internal::PoolLayoutCursor cursor(r, r.tree());
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
      "log52(6", "log"_l ^ KSubscriptL("52"_l) ^ KParenthesesRightTempL("6"_l));

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

typedef void (Poincare::Internal::PoolLayoutCursor::*AddLayoutPointer)(
    Context* context);
typedef void (*CursorAddLayout)(Poincare::Internal::PoolLayoutCursor* cursor,
                                Context* context,
                                AddLayoutPointer optionalAddLayoutFunction);

void assert_apply_beautification_after_layout_insertion(
    CursorAddLayout layoutInsertionFunction,
    AddLayoutPointer optionalAddLayoutFunction = nullptr) {
  Layout r = KRackL();
  Poincare::Internal::PoolLayoutCursor c(r, r.tree());
  Shared::GlobalContext context;
  c.insertText("pi", &context);
  (*layoutInsertionFunction)(&c, &context, optionalAddLayoutFunction);
  if (optionalAddLayoutFunction == &Poincare::Internal::PoolLayoutCursor::
                                       addFractionLayoutAndCollapseSiblings) {
    // Check numerator of created fraction
    quiz_assert(
        c.rootRack()->child(0)->child(0)->child(0)->treeIsIdenticalTo("π"_cl));
  } else {
    quiz_assert(c.rootRack()->child(0)->treeIsIdenticalTo("π"_cl));
  }
}

QUIZ_CASE(poincare_input_beautification_after_inserting_layout) {
  AddLayoutPointer layoutInsertionFunction[] = {
      &Poincare::Internal::PoolLayoutCursor::
          addFractionLayoutAndCollapseSiblings,
      /* addEmptyExponentialLayout inserts 2 layouts so it's not beautified.
       * Not a problem until it is reported by users as being a problem .. */
      // &Poincare::Internal::LayoutBufferCursor::addEmptyExponentialLayout,
      &Poincare::Internal::PoolLayoutCursor::addEmptyPowerLayout,
      &Poincare::Internal::PoolLayoutCursor::addEmptySquareRootLayout,
      &Poincare::Internal::PoolLayoutCursor::addEmptySquarePowerLayout,
      &Poincare::Internal::PoolLayoutCursor::addEmptyTenPowerLayout,
      &Poincare::Internal::PoolLayoutCursor::addEmptyMatrixLayout};
  int numberOfFunctions = std::size(layoutInsertionFunction);

  for (int i = 0; i < numberOfFunctions; i++) {
    assert_apply_beautification_after_layout_insertion(
        [](Poincare::Internal::PoolLayoutCursor* cursor, Context* context,
           AddLayoutPointer optionalAddLayoutFunction) {
          (cursor->*optionalAddLayoutFunction)(context);
        },
        layoutInsertionFunction[i]);
  }
}

QUIZ_CASE(poincare_input_beautification_derivative) {
  const Layout firstOrderDerivative =
      KRackL(KDiffL("x"_l, KRackL(), "1"_l, KRackL()));
  const Layout nthOrderDerivative =
      KRackL(KNthDiffL("x"_l, KRackL(), KRackL(), KRackL()));

  // Test d/dx()->diff()
  {
    Layout l = KRackL(KFracL("d"_l, "dx"_l));
    Poincare::Internal::PoolLayoutCursor c(l, l.tree());
    c.insertText("(");
    quiz_assert(c.rootRack()->treeIsIdenticalTo(firstOrderDerivative.tree()));
  }

  // Test d/dx^▯->d^▯/dx^▯
  {
    Layout l = firstOrderDerivative;
    Poincare::Internal::PoolLayoutCursor c(l, l.tree()->child(0)->child(0));
    c.addEmptyPowerLayout(nullptr);
    quiz_assert(c.rootRack()->treeIsIdenticalTo(nthOrderDerivative.tree()));
  }
}
