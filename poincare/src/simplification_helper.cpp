#include <poincare/simplification_helper.h>
#include <poincare/undefined.h>

namespace Poincare {

//TODO: , Preferences::ComplexFormat complexFormat?
Expression SimplificationHelper::Map(const Expression & e, Context & context, Preferences::AngleUnit angleUnit) {
  //TODO LEA
#if 0
  // TODO Use clones
  assert(e->numberOfChildren() == 1 && e->childAtIndex(0)->type() == ExpressionNode::Type::Matrix);
  Expression c = e.childAtIndex(0);
  Matrix matrix = Matrix::Builder();
  for (int i = 0; i < c->numberOfChildren(); i++) {
    Expression f = e.replaceChildAtIndexInPlace(0, e.childAtIndex(0).childAtIndex(i));
    matrix.addChildAtIndexInPlace(f, i, i);
    f.shallowReduce(context, complexFormat, angleUnit);
  }
  replaceWithInPlace(matrix);
  return matrix.shallowReduce(context, complexFormat, angleUnit);
#endif
  return Undefined::Builder();
}

}
