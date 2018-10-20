#include <quiz.h>
#include <poincare_layouts.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"
#include "./tree/helpers.h"

using namespace Poincare;

QUIZ_CASE(poincare_create_all_layouts) {
  EmptyLayout e0;
  AbsoluteValueLayout e1(e0);
  CharLayout e2('a');
  BinomialCoefficientLayout e3(e1, e2);
  CeilingLayout e4(e3);
  RightParenthesisLayout e5;
  RightSquareBracketLayout e6;
  CondensedSumLayout e7(e4, e5, e6);
  ConjugateLayout e8(e7);
  LeftParenthesisLayout e10;
  FloorLayout e11(e10);
  FractionLayout e12(e8, e11);
  HorizontalLayout e13;
  LeftSquareBracketLayout e14;
  IntegralLayout e15(e12, e13, e14);
  NthRootLayout e16(e15);
  MatrixLayout e17;
  EmptyLayout e18;
  EmptyLayout e19;
  EmptyLayout e20;
  ProductLayout e21(e18, e19, e20);
  EmptyLayout e22;
  EmptyLayout e23;
  EmptyLayout e24;
  SumLayout e25(e22, e23, e24);
  VerticalOffsetLayout e26(e25, VerticalOffsetLayoutNode::Type::Superscript);
}
