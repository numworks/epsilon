#include <apps/shared/global_context.h>
#include <poincare/matrix.h>

#include "helper.h"

using namespace Poincare;

static inline void assert_has_rank(const char *exp, int rank) {
  Shared::GlobalContext context;
  Expression e = parse_expression(exp, &context, false);
  quiz_assert(e.type() == ExpressionNode::Type::Matrix);
  Matrix *m = reinterpret_cast<Matrix *>(&e);
  quiz_assert(rank == m->rank(&context, Preferences::ComplexFormat::Cartesian,
                              Preferences::AngleUnit::Radian,
                              Preferences::UnitFormat::Metric,
                              ReductionTarget::SystemForApproximation, true));
}

QUIZ_CASE(poincare_matrix_rank) {
  assert_has_rank("[[1,2,3][1,3,4][1,4,6]]", 3);
  assert_has_rank("[[0,0,0][0,0,0][0,0,0]]", 0);
  assert_has_rank("[[1,0,0][0,1,0][0,0,0][0,0,0][1,1,1][0,0,1]]", 3);
  assert_has_rank("[[1,-1,0][0,1,2][0,1,2][0,1,2][0,1,2][0,1,2]]", 2);
  assert_has_rank("[[1,0,0,0,0,0][-1,1,1,1,1,1][0,2,2,2,2,2]]", 2);
}
