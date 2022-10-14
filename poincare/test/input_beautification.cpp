#include <poincare_layouts.h>
#include <apps/shared/global_context.h>
#include "helper.h"

using namespace Poincare;

void assert_inserted_text_turns_into(const char * textToInsert, Layout expectedLayout, const char * textRightOfInsertedText = "") {
  HorizontalLayout horizontalLayout = HorizontalLayout::Builder();
  LayoutCursor cursor(horizontalLayout);
  Shared::GlobalContext context;
  cursor.insertText(textRightOfInsertedText, &context, false, true);
  cursor.insertText(textToInsert, &context);
  quiz_assert(horizontalLayout.isIdenticalTo(expectedLayout));
}

QUIZ_CASE(poincare_input_beautification_after_inserting_text) {
  Layout l;

  // Beautify when input
  constexpr static const char * text0 = "\"\" '\" <= >= -> *";
  l = HorizontalLayout::Builder({
        CodePointLayout::Builder('\''),
        CodePointLayout::Builder(' '),
        CodePointLayout::Builder(UCodePointDegreeSign),
        CodePointLayout::Builder(' '),
        CodePointLayout::Builder(UCodePointInferiorEqual),
        CodePointLayout::Builder(' '),
        CodePointLayout::Builder(UCodePointSuperiorEqual),
        CodePointLayout::Builder(' '),
        CodePointLayout::Builder(UCodePointRightwardsArrow),
        CodePointLayout::Builder(' '),
        CodePointLayout::Builder(UCodePointMultiplicationSign)
      });
  assert_inserted_text_turns_into(text0, l);

  // Beautify when followed by non identifier
  constexpr static const char * text1 = "pi+theta+inf+";
  l = HorizontalLayout::Builder({
        CodePointLayout::Builder(UCodePointGreekSmallLetterPi),
        CodePointLayout::Builder('+'),
        CodePointLayout::Builder(UCodePointGreekSmallLetterTheta),
        CodePointLayout::Builder('+'),
        CodePointLayout::Builder(UCodePointInfinity),
        CodePointLayout::Builder('+')
      });
  assert_inserted_text_turns_into(text1, l);

  // Parse identifiers implicit multiplication
  constexpr static const char * text2 = "xpi+thetainf+";
  l = HorizontalLayout::Builder({
        CodePointLayout::Builder('x'),
        CodePointLayout::Builder(UCodePointGreekSmallLetterPi),
        CodePointLayout::Builder('+'),
        CodePointLayout::Builder(UCodePointGreekSmallLetterTheta),
        CodePointLayout::Builder(UCodePointInfinity),
        CodePointLayout::Builder('+')
      });
  assert_inserted_text_turns_into(text2, l);

  // Correctly beautify when parenthesed
  constexpr static const char * text3 = "pi))";
  l = HorizontalLayout::Builder({
        ParenthesisLayout::Builder(
          HorizontalLayout::Builder(
            ParenthesisLayout::Builder(
               HorizontalLayout::Builder(
                CodePointLayout::Builder(UCodePointGreekSmallLetterPi)
               )
            )
          )
        )
      });
  assert_inserted_text_turns_into(text3, l);

  constexpr static const char * text3bis = "pi)+(theta";
  l = HorizontalLayout::Builder({
        ParenthesisLayout::Builder(
          HorizontalLayout::Builder(
            CodePointLayout::Builder(UCodePointGreekSmallLetterPi)
          )
        ),
        CodePointLayout::Builder('+'),
        ParenthesisLayout::Builder(
          HorizontalLayout::Builder(
            CodePointLayout::Builder(UCodePointGreekSmallLetterTheta)
          )
        )
      });
  assert_inserted_text_turns_into(text3bis, l);

  // Do not alter normal behaviour
  constexpr static const char * text4 = "((4))(";
  l = HorizontalLayout::Builder({
        ParenthesisLayout::Builder(
          HorizontalLayout::Builder(
            ParenthesisLayout::Builder(
              HorizontalLayout::Builder(
                CodePointLayout::Builder('4')
              )
            )
          )
        ),
        ParenthesisLayout::Builder(
          HorizontalLayout::Builder(EmptyLayout::Builder())
        )
      });
  assert_inserted_text_turns_into(text4, l);

  // Correctly beautify pipe key
  constexpr static const char * text5 = "|3+4|+5+|6+7";
  l = HorizontalLayout::Builder({
        AbsoluteValueLayout::Builder(
          HorizontalLayout::Builder({
            CodePointLayout::Builder('3'),
            CodePointLayout::Builder('+'),
            CodePointLayout::Builder('4')
          })
        ),
        CodePointLayout::Builder('+'),
        CodePointLayout::Builder('5'),
        CodePointLayout::Builder('+'),
        AbsoluteValueLayout::Builder(HorizontalLayout::Builder(CodePointLayout::Builder('6'))),
        CodePointLayout::Builder('+'),
        CodePointLayout::Builder('7')
      });
  assert_inserted_text_turns_into(text5, l);

  // 5+| inserted left of 6+7
  constexpr static const char * text6Left = "5+|";
  constexpr static const char * text6Right = "6+7";
  l = HorizontalLayout::Builder({
        CodePointLayout::Builder('5'),
        CodePointLayout::Builder('+'),
        AbsoluteValueLayout::Builder(HorizontalLayout::Builder(CodePointLayout::Builder('6'))),
        CodePointLayout::Builder('+'),
        CodePointLayout::Builder('7')
      });
  assert_inserted_text_turns_into(text6Left, l, text6Right);

  // Beautify logN()
  constexpr static const char * text7 = "log52(6)";
  l = HorizontalLayout::Builder({
        CodePointLayout::Builder('l'),
        CodePointLayout::Builder('o'),
        CodePointLayout::Builder('g'),
        VerticalOffsetLayout::Builder(
          HorizontalLayout::Builder(
            CodePointLayout::Builder('5'),
            CodePointLayout::Builder('2')
          ),
          VerticalOffsetLayoutNode::Position::Subscript
        ),
        ParenthesisLayout::Builder(HorizontalLayout::Builder(CodePointLayout::Builder('6')))
      });
  assert_inserted_text_turns_into(text7, l);

  // Absorb arguments
  constexpr static const char * text8Left = "floor(";
  constexpr static const char * text8Right = "4+6";
  l = HorizontalLayout::Builder({
        FloorLayout::Builder(
          HorizontalLayout::Builder(
            CodePointLayout::Builder('4'),
            CodePointLayout::Builder('+'),
            CodePointLayout::Builder('6')
          )
        )
      });
  assert_inserted_text_turns_into(text8Left, l, text8Right);

  // Absorb multiple arguments
  constexpr static const char * text8bisLeft = "root(";
  constexpr static const char * text8bisRight = "4,6";
  l = HorizontalLayout::Builder({
        NthRootLayout::Builder(
          HorizontalLayout::Builder(
            CodePointLayout::Builder('4')
          ),
          HorizontalLayout::Builder(
            CodePointLayout::Builder('6')
          )
        )
      });
  assert_inserted_text_turns_into(text8bisLeft, l, text8bisRight);

  // Do not beautify when too many arguments
  constexpr static const char * text9 = "floor(5,6)";
  l = HorizontalLayout::Builder({
        CodePointLayout::Builder('f'),
        CodePointLayout::Builder('l'),
        CodePointLayout::Builder('o'),
        CodePointLayout::Builder('o'),
        CodePointLayout::Builder('r'),
        ParenthesisLayout::Builder(
          HorizontalLayout::Builder(
            CodePointLayout::Builder('5'),
            CodePointLayout::Builder(','),
            CodePointLayout::Builder('6')
          )
        )
      });
  assert_inserted_text_turns_into(text9, l);

  // Test all functions
  constexpr static const char * text10 = "abs(";
  l = HorizontalLayout::Builder({
    AbsoluteValueLayout::Builder(EmptyLayout::Builder())
  });
  assert_inserted_text_turns_into(text10, l);

  constexpr static const char * text11 = "binomial(";
  l = HorizontalLayout::Builder({
    BinomialCoefficientLayout::Builder(EmptyLayout::Builder(), EmptyLayout::Builder())
  });
  assert_inserted_text_turns_into(text11, l);

  constexpr static const char * text12 = "ceil(";
  l = HorizontalLayout::Builder({
    CeilingLayout::Builder(EmptyLayout::Builder())
  });
  assert_inserted_text_turns_into(text12, l);

  constexpr static const char * text13 = "conj(";
  l = HorizontalLayout::Builder({
    ConjugateLayout::Builder(EmptyLayout::Builder())
  });
  assert_inserted_text_turns_into(text13, l);

  constexpr static const char * text14 = "diff(";
  l = HorizontalLayout::Builder({
        FirstOrderDerivativeLayout::Builder(EmptyLayout::Builder(),CodePointLayout::Builder('x'),EmptyLayout::Builder())
      });
  assert_inserted_text_turns_into(text14, l);

  constexpr static const char * text15 = "exp(";
  l = HorizontalLayout::Builder({
        CodePointLayout::Builder('e'),
        VerticalOffsetLayout::Builder(EmptyLayout::Builder(), VerticalOffsetLayoutNode::Position::Superscript)
      });
  assert_inserted_text_turns_into(text15, l);

  constexpr static const char * text16 = "floor(";
  l = HorizontalLayout::Builder({
    FloorLayout::Builder(EmptyLayout::Builder())
  });
  assert_inserted_text_turns_into(text16, l);

  constexpr static const char * text17 = "norm(";
  l = HorizontalLayout::Builder({
    VectorNormLayout::Builder(EmptyLayout::Builder())
  });
  assert_inserted_text_turns_into(text17, l);

  constexpr static const char * text18 = "root(";
  l = HorizontalLayout::Builder({
    NthRootLayout::Builder(EmptyLayout::Builder(), EmptyLayout::Builder())
  });
  assert_inserted_text_turns_into(text18, l);

  constexpr static const char * text19 = "sqrt(";
  l = HorizontalLayout::Builder({
    NthRootLayout::Builder(EmptyLayout::Builder())
  });
  assert_inserted_text_turns_into(text19, l);

  constexpr static const char * text20 = "piecewise(";
  PiecewiseOperatorLayout pLayout = PiecewiseOperatorLayout::Builder();
  pLayout.addRow(EmptyLayout::Builder());
  pLayout.setDimensions(1, 2);
  pLayout.startEditing();
  l = HorizontalLayout::Builder({
    pLayout
  });
  assert_inserted_text_turns_into(text20, l);

  constexpr static const char * text21 = "int(";
  l = HorizontalLayout::Builder({
        IntegralLayout::Builder(EmptyLayout::Builder(),CodePointLayout::Builder('x'),EmptyLayout::Builder(),EmptyLayout::Builder())
      });
  assert_inserted_text_turns_into(text21, l);

  // Skip empty arguments
  constexpr static const char * text22 = "int(,x,4)";
  l = HorizontalLayout::Builder({
        IntegralLayout::Builder(EmptyLayout::Builder(),HorizontalLayout::Builder(CodePointLayout::Builder('x')),HorizontalLayout::Builder(CodePointLayout::Builder('4')),EmptyLayout::Builder())
      });
  assert_inserted_text_turns_into(text22, l);

  constexpr static const char * text22bis = "root(,4)";
  l = HorizontalLayout::Builder({
        NthRootLayout::Builder(EmptyLayout::Builder(),HorizontalLayout::Builder(CodePointLayout::Builder('4')))
      });
  assert_inserted_text_turns_into(text22bis, l);

  // Empty variable is replaced with default argument in parametered expression
  constexpr static const char * text23 = "int(2,,4)";
  l = HorizontalLayout::Builder({
        IntegralLayout::Builder(HorizontalLayout::Builder(CodePointLayout::Builder('2')),CodePointLayout::Builder('x'),HorizontalLayout::Builder(CodePointLayout::Builder('4')),EmptyLayout::Builder())
      });
  assert_inserted_text_turns_into(text23, l);

  // Parse identifiers implicit multiplication
  constexpr static const char * text24 = "pixsqrt(";
  l = HorizontalLayout::Builder({
    CodePointLayout::Builder(UCodePointGreekSmallLetterPi),
    CodePointLayout::Builder('x'),
    NthRootLayout::Builder(EmptyLayout::Builder())
  });
  assert_inserted_text_turns_into(text24, l);

  // Beautifiy imbricated functions
  constexpr static const char * text25 = "sqrt(floor()+conj(";
  l = HorizontalLayout::Builder({
        NthRootLayout::Builder(
          HorizontalLayout::Builder(
            FloorLayout::Builder(
              EmptyLayout::Builder()
            ),
            CodePointLayout::Builder('+'),
            ConjugateLayout::Builder(
              EmptyLayout::Builder()
            )
          )
        )
      });
  assert_inserted_text_turns_into(text25, l);
}

typedef void (LayoutCursor::*AddLayoutPointer)(Context * context);
typedef void (*CursorAddLayout)(LayoutCursor * cursor, Context * context, AddLayoutPointer optionalAddLayoutFunction);

void assert_apply_beautification_after_layout_insertion(CursorAddLayout layoutInsertionFunction, AddLayoutPointer optionalAddLayoutFunction = nullptr) {
  HorizontalLayout horizontalLayout = HorizontalLayout::Builder();
  LayoutCursor cursor(horizontalLayout);
  Shared::GlobalContext context;
  cursor.insertText("pi", &context);
  (*layoutInsertionFunction)(&cursor, &context, optionalAddLayoutFunction);
  Layout piCodePoint = CodePointLayout::Builder(UCodePointGreekSmallLetterPi);
  if (optionalAddLayoutFunction == &LayoutCursor::addFractionLayoutAndCollapseSiblings) {
    // Check numerator of created fraction
    quiz_assert(horizontalLayout.childAtIndex(0).childAtIndex(0).childAtIndex(0).isIdenticalTo(piCodePoint));
  } else {
    quiz_assert(horizontalLayout.childAtIndex(0).isIdenticalTo(piCodePoint));
  }
}

QUIZ_CASE(poincare_input_beautification_after_inserting_layout) {
  AddLayoutPointer layoutInsertionFunction[] = {&LayoutCursor::addFractionLayoutAndCollapseSiblings, &LayoutCursor::addEmptyExponentialLayout,  &LayoutCursor::addEmptyPowerLayout, &LayoutCursor::addEmptySquareRootLayout, &LayoutCursor::addEmptySquarePowerLayout, &LayoutCursor::addEmptyTenPowerLayout, &LayoutCursor::addEmptyMatrixLayout};
  int numberOfFunctions = sizeof(layoutInsertionFunction)/sizeof(AddLayoutPointer);

  for (int i = 0; i < numberOfFunctions; i++) {
    assert_apply_beautification_after_layout_insertion(
      [](LayoutCursor * cursor, Context * context, AddLayoutPointer optionalAddLayoutFunction) {
        (cursor->*optionalAddLayoutFunction)(context);
      },
      layoutInsertionFunction[i]);
  }

  assert_apply_beautification_after_layout_insertion(
    [](LayoutCursor * cursor, Context * context, AddLayoutPointer optionalAddLayoutFunction) {
      Layout l = HorizontalLayout::Builder(CodePointLayout::Builder('a'), NthRootLayout::Builder(EmptyLayout::Builder()));
      cursor->addLayoutAndMoveCursor(l, context);
    });
}

QUIZ_CASE(poincare_input_beautification_derivative) {
  // Test d/dx()->diff()
  HorizontalLayout horizontalLayout = HorizontalLayout::Builder(
    FractionLayout::Builder(
      HorizontalLayout::Builder(CodePointLayout::Builder('d')),
      HorizontalLayout::Builder(CodePointLayout::Builder('d'), CodePointLayout::Builder('x'))
    )
  );
  LayoutCursor cursor(horizontalLayout);
  Shared::GlobalContext context;
  cursor.insertText("(", &context);
  quiz_assert(horizontalLayout.isIdenticalTo(HorizontalLayout::Builder(FirstOrderDerivativeLayout::Builder(EmptyLayout::Builder(),CodePointLayout::Builder('x'),EmptyLayout::Builder()))));

  // Test d/dx^2->d^2/dx^2
  cursor.setLayout(horizontalLayout.childAtIndex(0).childAtIndex(1));
  cursor.addEmptyPowerLayout(&context);
  quiz_assert(horizontalLayout.isIdenticalTo(HorizontalLayout::Builder(HigherOrderDerivativeLayout::Builder(EmptyLayout::Builder(),HorizontalLayout::Builder(CodePointLayout::Builder('x')),EmptyLayout::Builder(),EmptyLayout::Builder()))));
}
