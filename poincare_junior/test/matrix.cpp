#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/expression/matrix.h>

#include "helper.h"
using namespace PoincareJ;

QUIZ_CASE(pcj_matrix) {
  const Tree* u = KMatrix<1, 2>()(1_e, 2_e);
  QUIZ_ASSERT(Matrix::NumberOfRows(u) == 1);
  QUIZ_ASSERT(Matrix::NumberOfColumns(u) == 2);
  QUIZ_ASSERT(Matrix::ChildAtIndex(u, 0, 1)->treeIsIdenticalTo(2_e));
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
  SharedEditionPool->flush();
}
