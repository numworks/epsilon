#include <apps/shared/global_context.h>
#include <poincare/src/expression/matrix.h>

#include "helper.h"

using namespace Poincare;

static inline void assert_has_rank(const char *exp, int rank) {
  Shared::GlobalContext context;
  Internal::Tree *e = parse_expression(exp, &context);
  quiz_assert(e->isMatrix());
  quiz_assert(rank == Internal::Matrix::Rank(e));
  e->removeTree();
}

QUIZ_CASE(poincare_matrix_rank) {
  assert_has_rank("[[1,2,3][1,3,4][1,4,6]]", 3);
  assert_has_rank("[[0,0,0][0,0,0][0,0,0]]", 0);
  assert_has_rank("[[1,0,0][0,1,0][0,0,0][0,0,0][1,1,1][0,0,1]]", 3);
  assert_has_rank("[[1,-1,0][0,1,2][0,1,2][0,1,2][0,1,2][0,1,2]]", 2);
  assert_has_rank("[[1,0,0,0,0,0][-1,1,1,1,1,1][0,2,2,2,2,2]]", 2);
  assert_has_rank("[[1,2,3][1,3,x][1,4,6]]", -1);
  assert_has_rank("[[1,1,0][0,0,0][0,1,x]]", -1);
}

QUIZ_CASE(poincare_matrix_undef) {
  assert_expression_approximates_to<float>("cross([[0]],[[0]])", "undef");
  assert_expression_approximates_to<float>("det(cross([[0]],[[0]]))", "undef");
  assert_expression_approximates_to<float>("dim(cross([[0]],[[0]]))", "undef");
  assert_expression_approximates_to<float>("inverse(cross([[0]],[[0]]))",
                                           "undef");
  assert_expression_approximates_to<float>("norm(cross([[0]],[[0]]))", "undef");
  assert_expression_approximates_to<float>("ref(cross([[0]],[[0]]))", "undef");
  assert_expression_approximates_to<float>("trace(cross([[0]],[[0]]))",
                                           "undef");
  assert_expression_approximates_to<float>("transpose(cross([[0]],[[0]]))",
                                           "undef");
}
