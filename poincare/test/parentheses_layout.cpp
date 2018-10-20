#include <quiz.h>
#include <poincare_layouts.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_parenthesis_layout_size) {
  /*      3
   * (2+(---)6)1
   *      4
   * Assert that the first and last parentheses have the same size.
   */
  HorizontalLayout layout = HorizontalLayout();
  LeftParenthesisLayout leftPar = LeftParenthesisLayout();
  RightParenthesisLayout rightPar = RightParenthesisLayout();
  layout.addChildAtIndex(leftPar, 0, 0, nullptr);
  layout.addChildAtIndex(CharLayout('2'), 1, 1, nullptr);
  layout.addChildAtIndex(CharLayout('+'), 2, 2, nullptr);
  layout.addChildAtIndex(LeftParenthesisLayout(), 3, 3, nullptr);
  layout.addChildAtIndex(FractionLayout(
        CharLayout('3'),
        CharLayout('4')),
      4, 4, nullptr);
  layout.addChildAtIndex(RightParenthesisLayout(), 4, 4, nullptr);
  layout.addChildAtIndex(CharLayout('6'), 5, 5, nullptr);
  layout.addChildAtIndex(rightPar, 7, 7, nullptr);
  layout.addChildAtIndex(CharLayout('1'), 8, 8, nullptr);
  quiz_assert(leftPar.layoutSize().height() == rightPar.layoutSize().height());
}
