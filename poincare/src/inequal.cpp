#include <poincare/inequal.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/horizontal_layout.h>
#include <poincare/simplification_helper.h>

namespace Poincare {

Expression InequalNode::shallowReduce(ReductionContext reductionContext) {
  return Inequal(this).shallowReduce();
}

Expression Inequal::shallowReduce() {
  {
    Expression e = SimplificationHelper::shallowReduceUndefined(*this);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  return *this;
}

}
