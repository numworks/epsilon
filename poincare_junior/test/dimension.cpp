#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/expression/dimension.h>
#include <poincare_junior/src/layout/parsing/rack_parser.h>

#include "helper.h"

using namespace PoincareJ;

Dimension dim(const char* input) {
  EditionReference inputLayout = Layout::EditionPoolTextToLayout(input);
  EditionReference expression = RackParser(inputLayout).parse();
  quiz_assert(!expression.isUninitialized());
  inputLayout->removeTree();
  Dimension dim = Dimension::DeepCheckDimensions(expression);
  if (!dim.isInvalid()) {
    assert(dim == Dimension::GetDimension(expression));
  }
  expression->removeTree();
  return dim;
}

QUIZ_CASE(pcj_dimension) {
  auto Invalid = Dimension::Invalid();
  auto Scalar = Dimension::Scalar();
  auto Matrix = Dimension::Matrix;

  QUIZ_ASSERT(dim("1") == Scalar);
  QUIZ_ASSERT(dim("cos(sin(1+3))*2^3") == Scalar);
  QUIZ_ASSERT(dim("[[1][3]]") == Matrix(2, 1));
  QUIZ_ASSERT(dim("[[1][[[2]]]]") == Invalid);
  QUIZ_ASSERT(dim("ref([[1,2][3,4]])") == Matrix(2, 2));
  QUIZ_ASSERT(dim("[[1,2][3,4]]+[[2]]") == Invalid);
  QUIZ_ASSERT(dim("inverse(identity(2))") == Matrix(2, 2));
  QUIZ_ASSERT(dim("cross([[1,2,3]],[[1,2,3]])") == Matrix(1, 3));
  QUIZ_ASSERT(dim("cos([[2]])") == Invalid);
  QUIZ_ASSERT(dim("transpose([[1,2]])*[[1,2,3]]") == Matrix(2, 3));

  QUIZ_ASSERT(SharedEditionPool->numberOfTrees() == 0);
}
