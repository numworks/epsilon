#include <poincare/inferior_equal.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/horizontal_layout.h>
#include <poincare/simplification_helper.h>

namespace Poincare {

Expression InferiorEqualNode::shallowReduce(ReductionContext reductionContext) {
  return InferiorEqual(this).shallowReduce();
}

Expression InferiorEqual::shallowReduce() {
  {
    Expression e = SimplificationHelper::shallowReduceUndefined(*this);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  return *this;
}

}
