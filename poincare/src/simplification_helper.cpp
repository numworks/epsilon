#include <poincare/simplification_helper.h>

#if MATRIX_EXACT_REDUCING
namespace Poincare {

Expression SimplificationHelper::Map(Expression e, Context & context, Preferences::AngleUnit angleUnit) {
  assert(e->numberOfChildren() == 1 && e->childAtIndex(0)->type() == Expression::Type::Matrix);
  Expression c = e.childAtIndex(0);
  Matrix matrix;
  for (int i = 0; i < c->numberOfChildren(); i++) {
    Expression f = e.replaceChildAtIndexInPlace(0, e.childAtIndex(0).childAtIndex(i));
    matrix.addChildAtIndexInPlace(f.shallowReduce(context, angleUnit), i, i);
  }
  return matrix.shallowReduce(context, angleUnit);
}

}
#endif
