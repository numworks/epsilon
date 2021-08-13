#include <poincare/inferior.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/horizontal_layout.h>
#include <poincare/simplification_helper.h>

namespace Poincare {

Expression InferiorNode::shallowReduce(ReductionContext reductionContext) {
  return Inferior(this).shallowReduce();
}

Expression Inferior::shallowReduce() {
  {
    Expression e = SimplificationHelper::shallowReduceUndefined(*this);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  return *this;
}

}
