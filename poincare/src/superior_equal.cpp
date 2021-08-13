#include <poincare/superior_equal.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/horizontal_layout.h>
#include <poincare/simplification_helper.h>

namespace Poincare {

Expression SuperiorEqualNode::shallowReduce(ReductionContext reductionContext) {
  return SuperiorEqual(this).shallowReduce();
}

Expression SuperiorEqual::shallowReduce() {
  {
    Expression e = SimplificationHelper::shallowReduceUndefined(*this);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  return *this;
}

}
