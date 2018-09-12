#include <poincare/simplification_helper.h>

#if MATRIX_EXACT_REDUCING
namespace Poincare {

// TODO Use clones
Expression SimplificationHelper::Map(const Expression & e, Context & context, Preferences::AngleUnit angleUnit) {
  assert(e->numberOfChildren() == 1 && e->childAtIndex(0)->type() == ExpressionNode::Type::Matrix);
  Expression c = e.childAtIndex(0);
  Matrix matrix;
  for (int i = 0; i < c->numberOfChildren(); i++) {
    Expression f = e.replaceChildAtIndexInPlace(0, e.childAtIndex(0).childAtIndex(i));
    matrix.addChildAtIndexInPlace(f, i, i);
    f.shallowReduce(context, angleUnit);
  }
  replaceWithInPlace(matrix);
  return matrix.shallowReduce(context, angleUnit);
}

}
#endif
