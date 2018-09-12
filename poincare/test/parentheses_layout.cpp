#include <quiz.h>
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
  HorizontalLayoutRef layout = HorizontalLayoutRef();
  LeftParenthesisLayoutRef leftPar = LeftParenthesisLayoutRef();
  RightParenthesisLayoutRef rightPar = RightParenthesisLayoutRef();
  layout.addChildAtIndex(leftPar, 0, 0, nullptr);
  layout.addChildAtIndex(CharLayoutRef('2'), 1, 1, nullptr);
  layout.addChildAtIndex(CharLayoutRef('+'), 2, 2, nullptr);
  layout.addChildAtIndex(LeftParenthesisLayoutRef(), 3, 3, nullptr);
  layout.addChildAtIndex(FractionLayoutRef(
        CharLayoutRef('3'),
        CharLayoutRef('4')),
      4, 4, nullptr);
  layout.addChildAtIndex(RightParenthesisLayoutRef(), 4, 4, nullptr);
  layout.addChildAtIndex(CharLayoutRef('6'), 5, 5, nullptr);
  layout.addChildAtIndex(rightPar, 7, 7, nullptr);
  layout.addChildAtIndex(CharLayoutRef('1'), 8, 8, nullptr);
  quiz_assert(leftPar.layoutSize().height() == rightPar.layoutSize().height());
}
