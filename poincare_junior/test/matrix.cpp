#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/expression/matrix.h>

#include "helper.h"
using namespace PoincareJ;

QUIZ_CASE(pcj_matrix) {
  const Tree* u = KMatrix<1, 2>()(1_e, 2_e);
  QUIZ_ASSERT(Matrix::NumberOfRows(u) == 1);
  QUIZ_ASSERT(Matrix::NumberOfColumns(u) == 2);
  const Tree* v = KMatrix<1, 2>()(3_e, 4_e);
  assert_trees_are_equal(Matrix::Addition(u, v), KMatrix<1, 2>()(4_e, 6_e));
}
