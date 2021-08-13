#include <poincare/superior.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/horizontal_layout.h>
#include <poincare/simplification_helper.h>

namespace Poincare {

Expression SuperiorNode::shallowReduce(ReductionContext reductionContext) {
  return Superior(this).shallowReduce();
}

Expression Superior::shallowReduce() {
  {
    Expression e = SimplificationHelper::shallowReduceUndefined(*this);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  return *this;
}

}
