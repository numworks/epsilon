#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/expression/matrix.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/layout/parsing/rack_parser.h>

#include "helper.h"
using namespace PoincareJ;

Tree* parse_matrix(const char* input) {
  EditionReference inputLayout = Layout::EditionPoolTextToLayout(input);
  EditionReference expression = RackParser(inputLayout).parse();
  quiz_assert(!expression.isUninitialized() &&
              expression->type() == BlockType::Matrix);
  inputLayout->removeTree();
  return expression;
}

QUIZ_CASE(pcj_matrix) {
  const Tree* u = KMatrix<1, 2>()(1_e, 2_e);
  QUIZ_ASSERT(Matrix::NumberOfRows(u) == 1);
  QUIZ_ASSERT(Matrix::NumberOfColumns(u) == 2);
  QUIZ_ASSERT(Matrix::Child(u, 0, 1)->treeIsIdenticalTo(2_e));
  const Tree* v = KMatrix<1, 2>()(3_e, 4_e);
  assert_trees_are_equal(Matrix::Addition(u, v), KMatrix<1, 2>()(4_e, 6_e));
  assert_trees_are_equal(Matrix::Identity(2_e),
                         KMatrix<2, 2>()(1_e, 0_e, 0_e, 1_e));

  const Tree* w1 = KMatrix<2, 3>()(1_e, 2_e, 3_e, 4_e, 5_e, 6_e);
  const Tree* w2 = KMatrix<3, 1>()(7_e, 8_e, 9_e);
  assert_trees_are_equal(Matrix::Multiplication(w1, w2),
                         KMatrix<2, 1>()(50_e, 122_e));
  assert_trees_are_equal(Matrix::Multiplication(w1, Matrix::Identity(3_e)), w1);

  assert_trees_are_equal(Matrix::Trace(Matrix::Identity(12_e)), 12_e);
  assert_trees_are_equal(Matrix::Transpose(w1),
                         KMatrix<3, 2>()(1_e, 4_e, 2_e, 5_e, 3_e, 6_e));
  SharedEditionPool->flush();

  assert_trees_are_equal(parse_matrix("[[1,2,3][4,5,6]]"), w1);

  Tree* m = parse_matrix("[[0,2,-1][5,6,7][12,11,10]]");
  Tree* det;
  Simplification::DeepSystematicReduce(m);
  Matrix::RowCanonize(m, false, &det);
  Tree* res = parse_matrix("[[1,11/12,5/6][0,1,-1/2][0,0,1]]");
  Simplification::DeepSystemProjection(res);
  Simplification::DeepSystematicReduce(res);
  assert_trees_are_equal(m, res);
  assert_trees_are_equal(det, 85_e);
}
