#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"
#include "./tree/helpers.h"

using namespace Poincare;

QUIZ_CASE(poincare_create_all_layouts) {
  EmptyLayoutReference e0;
  AbsoluteValueLayoutReference e1(e0);
  CharLayoutReference e2('a');
  BinomialCoefficientLayoutReference e3(e1, e2);
  CeilingLayoutReference e4(e3);
  RightParenthesisLayoutReference e5;
  RightSquareBracketLayoutReference e6;
  CondensedSumLayoutReference e7(e4, e5, e6);
  ConjugateLayoutReference e8(e7);
  LeftParenthesisLayoutReference e10;
  FloorLayoutReference e11(e10);
  FractionLayoutReference e12(e8, e11);
  HorizontalLayoutReference e13;
  LeftSquareBracketLayoutReference e14;
  IntegralLayoutReference e15(e12, e13, e14);
  NthRootLayoutReference e16(e15);
  MatrixLayoutReference e17;
  EmptyLayoutReference e18;
  EmptyLayoutReference e19;
  EmptyLayoutReference e20;
  ProductLayoutReference e21(e18, e19, e20);
  EmptyLayoutReference e22;
  EmptyLayoutReference e23;
  EmptyLayoutReference e24;
  SumLayoutReference e25(e22, e23, e24);
  VerticalOffsetLayoutReference e26(e25, VerticalOffsetLayoutNode::Type::Superscript);
}
