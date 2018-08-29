#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"
#include "./tree/helpers.h"

using namespace Poincare;

QUIZ_CASE(poincare_create_all_layouts) {
  EmptyLayoutRef e0;
  AbsoluteValueLayoutRef e1(e0);
  CharLayoutRef e2('a');
  BinomialCoefficientLayoutRef e3(e1, e2);
  CeilingLayoutRef e4(e3);
  RightParenthesisLayoutRef e5;
  RightSquareBracketLayoutRef e6;
  CondensedSumLayoutRef e7(e4, e5, e6);
  ConjugateLayoutRef e8(e7);
  LeftParenthesisLayoutRef e10;
  FloorLayoutRef e11(e10);
  FractionLayoutRef e12(e8, e11);
  HorizontalLayoutRef e13;
  LeftSquareBracketLayoutRef e14;
  IntegralLayoutRef e15(e12, e13, e14);
  NthRootLayoutRef e16(e15);
  MatrixLayoutRef e17;
  EmptyLayoutRef e18;
  EmptyLayoutRef e19;
  EmptyLayoutRef e20;
  ProductLayoutRef e21(e18, e19, e20);
  EmptyLayoutRef e22;
  EmptyLayoutRef e23;
  EmptyLayoutRef e24;
  SumLayoutRef e25(e22, e23, e24);
  VerticalOffsetLayoutRef e26(e25, VerticalOffsetLayoutNode::Type::Superscript);
}
