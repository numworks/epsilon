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
  HorizontalLayoutReference layout = HorizontalLayoutReference();
  LeftParenthesisLayoutReference leftPar = LeftParenthesisLayoutReference();
  RightParenthesisLayoutReference rightPar = RightParenthesisLayoutReference();
  layout.addChildAtIndex(leftPar, 0, 0, nullptr);
  layout.addChildAtIndex(CharLayoutReference('2'), 1, 1, nullptr);
  layout.addChildAtIndex(CharLayoutReference('+'), 2, 2, nullptr);
  layout.addChildAtIndex(LeftParenthesisLayoutReference(), 3, 3, nullptr);
  layout.addChildAtIndex(FractionLayoutReference(
        CharLayoutReference('3'),
        CharLayoutReference('4')),
      4, 4, nullptr);
  layout.addChildAtIndex(RightParenthesisLayoutReference(), 4, 4, nullptr);
  layout.addChildAtIndex(CharLayoutReference('6'), 5, 5, nullptr);
  layout.addChildAtIndex(rightPar, 7, 7, nullptr);
  layout.addChildAtIndex(CharLayoutReference('1'), 8, 8, nullptr);
  quiz_assert(leftPar.layoutSize().height() == rightPar.layoutSize().height());
}
