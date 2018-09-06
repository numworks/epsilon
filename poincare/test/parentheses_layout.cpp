#include <quiz.h>
#include <ion.h>
#include <poincare_layouts.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_parenthesis_layout_size) {
  /*      3
   * (2+(---)6)1
   *      4
   * Assert that the first and last parentheses have the same size.
   */
  HorizontalLayoutRef layout = HorizontalLayoutRef();
  LeftParenthesisLayout leftPar = LeftParenthesisLayoutRef();
  RightParenthesisLayout rightPar = RightParenthesisLayoutRef();
  layout.addChildAtIndexInPlace(leftPar, 0);
  layout.addChildAtIndexInPlace(CharLayoutRef('2'), 1);
  layout.addChildAtIndexInPlace(CharLayoutRef('+'), 2);
  layout.addChildAtIndexInPlace(LeftParenthesisLayoutRef(), 3);
  layout.addChildAtIndexInPlace(FractionLayoutRef(
        CharLayoutRef('3'),
        CharLayoutRef('4')),
      4);
  layout.addChildAtIndexInPlace(RightParenthesisLayoutRef(), 3);
  layout.addChildAtIndexInPlace(CharLayoutRef('6'), 5);
  layout.addChildAtIndexInPlace(rightPar, 7);
  layout.addChildAtIndexInPlace(CharLayoutRef('1'), 8);
  quiz_assert(leftPar->size().height() == rightPar->size().height());
}
