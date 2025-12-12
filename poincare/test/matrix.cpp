#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/matrix.h>
#include <poincare/src/expression/simplification.h>

#include "helper.h"

using namespace Poincare::Internal;

QUIZ_CASE(pcj_matrix) {
  const Tree* m1 = KMatrix<1, 2>(1_e, 2_e);
  QUIZ_ASSERT(Matrix::NumberOfRows(m1) == 1);
  QUIZ_ASSERT(Matrix::NumberOfColumns(m1) == 2);
  QUIZ_ASSERT(Matrix::Child(m1, 0, 1)->treeIsIdenticalTo(2_e));
  const Tree* m2 = KMatrix<1, 2>(3_e, 4_e);
  assert_trees_are_equal(Matrix::Addition(m1, m2), KMatrix<1, 2>(4_e, 6_e));
  assert_trees_are_equal(Matrix::Identity(2_e),
                         KMatrix<2, 2>(1_e, 0_e, 0_e, 1_e));

  const Tree* w1 = KMatrix<2, 3>(1_e, 2_e, 3_e, 4_e, 5_e, 6_e);
  const Tree* w2 = KMatrix<3, 1>(7_e, 8_e, 9_e);
  Tree* w2Clone = w2->cloneTree();
  QUIZ_ASSERT(Matrix::CanonizeAndRank(w2Clone) == 1);
  w2Clone->removeTree();
  assert_trees_are_equal(Matrix::Multiplication(w1, w2),
                         KMatrix<2, 1>(50_e, 122_e));
  assert_trees_are_equal(Matrix::Multiplication(w1, Matrix::Identity(3_e)), w1);

  assert_trees_are_equal(Matrix::Trace(Matrix::Identity(12_e)), 12_e);
  assert_trees_are_equal(Matrix::Transpose(w1),
                         KMatrix<3, 2>(1_e, 4_e, 2_e, 5_e, 3_e, 6_e));
  SharedTreeStack->flush();

  assert_trees_are_equal(parse("[[1,2,3][4,5,6]]"), w1);
}

static inline void assert_has_rank(const char* exp, int rank) {
  Tree* e = parse(exp);
  quiz_assert(e->isMatrix());
  quiz_assert(rank == Matrix::CanonizeAndRank(e));
  e->removeTree();
}

QUIZ_CASE(pcj_matrix_rank) {
  assert_has_rank("[[1,2,3][1,3,4][1,4,6]]", 3);
  assert_has_rank("[[0,0,0][0,0,0][0,0,0]]", 0);
  assert_has_rank("[[1,0,0][0,1,0][0,0,0][0,0,0][1,1,1][0,0,1]]", 3);
  assert_has_rank("[[1,-1,0][0,1,2][0,1,2][0,1,2][0,1,2][0,1,2]]", 2);
  assert_has_rank("[[1,0,0,0,0,0][-1,1,1,1,1,1][0,2,2,2,2,2]]", 2);
  assert_has_rank("[[1,2,3][1,3,x][1,4,6]]", -1);
  assert_has_rank("[[1,1,0][0,0,0][0,1,x]]", -1);
}
