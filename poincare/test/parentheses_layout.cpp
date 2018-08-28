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
  HorizontalLayout * layout = new HorizontalLayout();
  LeftParenthesisLayout leftPar = new LeftParenthesisLayout();
  RightParenthesisLayout rightPar = new RightParenthesisLayout();
  layout->addChildAtIndexInPlace(leftPar, 0);
  layout->addChildAtIndexInPlace(new CharLayout('2'), 1);
  layout->addChildAtIndexInPlace(new CharLayout('+'), 2);
  layout->addChildAtIndexInPlace(new LeftParenthesisLayout(), 3);
  layout->addChildAtIndexInPlace(new FractionLayout(
        new CharLayout('3'),
        new CharLayout('4')),
      4);
  layout->addChildAtIndexInPlace(new RightParenthesisLayout(), 3);
  layout->addChildAtIndexInPlace(new CharLayout('6'), 5);
  layout->addChildAtIndexInPlace(rightPar, 7);
  layout->addChildAtIndexInPlace(new CharLayout('1'), 8);
  quiz_assert(leftPar->size().height() == rightPar->size().height());
  delete layout;
}
