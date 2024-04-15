#include <poincare/src/expression/dimension.h>

#include "helper.h"

using namespace Poincare::Internal;

bool dim(const char* input, Dimension d = Dimension::Matrix(0, 0)) {
  Tree* expression = TextToTree(input);
  bool result = Dimension::DeepCheckDimensions(expression) &&
                d == Dimension::GetDimension(expression);
  expression->removeTree();
  return result;
}

bool len(const char* input, int n) {
  Tree* expression = TextToTree(input);
  bool result = Dimension::GetListLength(expression) == n;
  expression->removeTree();
  return result;
}

QUIZ_CASE(pcj_dimension) {
  auto Scalar = Dimension::Scalar();
  auto Matrix = Dimension::Matrix;
  auto Boolean = Dimension::Boolean();
  auto Point = Dimension::Point();
  QUIZ_ASSERT(dim("piecewise([[2]],True,[[3]])", Matrix(1, 1)));

  QUIZ_ASSERT(!dim("[[1][[[2]]]]"));
  QUIZ_ASSERT(!dim("[[1,2][3,4]]+[[2]]"));
  QUIZ_ASSERT(!dim("cos([[2]])"));
  QUIZ_ASSERT(!dim("1/[[1][3]]"));
  QUIZ_ASSERT(!dim("product([[k,2]], k, 1, n)"));
  QUIZ_ASSERT(!dim("(True, False)"));
  QUIZ_ASSERT(!dim("{2,(1,3)}"));

  QUIZ_ASSERT(dim("1", Scalar));
  QUIZ_ASSERT(dim("cos(sin(1+3))*2^3", Scalar));
  QUIZ_ASSERT(dim("[[1][3]]", Matrix(2, 1)));
  QUIZ_ASSERT(dim("[[1][3]]/3", Matrix(2, 1)));
  QUIZ_ASSERT(dim("ref([[1,2][3,4]])", Matrix(2, 2)));
  QUIZ_ASSERT(dim("inverse(identity(2))", Matrix(2, 2)));
  QUIZ_ASSERT(dim("cross([[1,2,3]],[[1,2,3]])", Matrix(1, 3)));
  QUIZ_ASSERT(dim("transpose([[1,2]])*[[1,2,3]]", Matrix(2, 3)));
  QUIZ_ASSERT(dim("dep([[k,2]], {})", Matrix(1, 2)));
  QUIZ_ASSERT(dim("sum([[k,2]], k, 1, n)", Matrix(1, 2)));
  QUIZ_ASSERT(dim("{}", Scalar));
  QUIZ_ASSERT(dim("sequence(k,k,3)", Scalar));

  QUIZ_ASSERT(dim("True and False", Boolean));
  QUIZ_ASSERT(!dim("0 and False"));
  QUIZ_ASSERT(dim("0 < 3 and False", Boolean));

  QUIZ_ASSERT(len("1", Dimension::k_nonListListLength));
  QUIZ_ASSERT(len("{1,2}", 2));
  QUIZ_ASSERT(len("2*cos({1,2})+3", 2));
  QUIZ_ASSERT(len("sequence(2*k+1,k,4)", 4));
  QUIZ_ASSERT(len("2+mean({1,2})", Dimension::k_nonListListLength));
  QUIZ_ASSERT(len("sort({1,2})", 2));
  QUIZ_ASSERT(len("{}", 0));

  QUIZ_ASSERT(dim("(2,3)", Point));
  QUIZ_ASSERT(dim("{(2,3)}", Point));
  QUIZ_ASSERT(dim("(2,{1,3})", Point));
  QUIZ_ASSERT(dim("piecewise((1,5),x<1,(x,y))", Point));
  QUIZ_ASSERT(dim("diff((x,2x), x, y)", Point));
  QUIZ_ASSERT(dim("dep((1,2), {(1,3),(3,3)})", Point));
  QUIZ_ASSERT(dim("sequence((k,2),k,3)", Point));
  QUIZ_ASSERT(dim("{(1,3), (2,4)}", Point));
  QUIZ_ASSERT(dim("sort({(1,3), (2,4)})", Point));
  QUIZ_ASSERT(dim("{(1,3), (2,4)}(1)", Point));
  QUIZ_ASSERT(dim("{(1,3), (2,4)}(0,1)", Point));
  QUIZ_ASSERT(dim("{(1,3), (2,4)}(1,0)", Scalar));
  QUIZ_ASSERT(dim("dim({(1,3), (2,4)})", Scalar));
  QUIZ_ASSERT(dim("sort(diff({(x,2x),(1,2)}, x, y))", Point));

  QUIZ_ASSERT(!dim("dep((1,2), {(1,3),3})"));
  QUIZ_ASSERT(!dim("(1,2)+(1,3)"));
  QUIZ_ASSERT(!dim("cos((1,2))"));
  QUIZ_ASSERT(!dim("{(1,3), (2,4)}((2,4))"));

  QUIZ_ASSERT(SharedTreeStack->numberOfTrees() == 0);
}
