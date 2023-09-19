#include <poincare_junior/src/expression/dimension.h>

#include "helper.h"

using namespace PoincareJ;

bool dim(const char* input, Dimension d = Dimension::Matrix(0, 0)) {
  Tree* expression = TextToTree(input);
  bool result = Dimension::DeepCheckDimensions(expression) &&
                d == Dimension::GetDimension(expression);
  expression->removeTree();
  return result;
}

QUIZ_CASE(pcj_dimension) {
  auto Scalar = Dimension::Scalar();
  auto Matrix = Dimension::Matrix;

  QUIZ_ASSERT(!dim("[[1][[[2]]]]"));
  QUIZ_ASSERT(!dim("[[1,2][3,4]]+[[2]]"));
  QUIZ_ASSERT(!dim("cos([[2]])"));
  QUIZ_ASSERT(!dim("1/[[1][3]]"));
  QUIZ_ASSERT(!dim("product([[k,2]], k, 1, n)"));

  QUIZ_ASSERT(dim("1", Scalar));
  QUIZ_ASSERT(dim("cos(sin(1+3))*2^3", Scalar));
  QUIZ_ASSERT(dim("[[1][3]]", Matrix(2, 1)));
  QUIZ_ASSERT(dim("[[1][3]]/3", Matrix(2, 1)));
  QUIZ_ASSERT(dim("ref([[1,2][3,4]])", Matrix(2, 2)));
  QUIZ_ASSERT(dim("inverse(identity(2))", Matrix(2, 2)));
  QUIZ_ASSERT(dim("cross([[1,2,3]],[[1,2,3]])", Matrix(1, 3)));
  QUIZ_ASSERT(dim("transpose([[1,2]])*[[1,2,3]]", Matrix(2, 3)));
  QUIZ_ASSERT(dim("sum([[k,2]], k, 1, n)", Matrix(1, 2)));

  QUIZ_ASSERT(SharedEditionPool->numberOfTrees() == 0);
}
