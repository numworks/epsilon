#include <poincare/equal.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/horizontal_layout.h>
#include <poincare/simplification_helper.h>

namespace Poincare {

Expression EqualNode::shallowReduce(ReductionContext reductionContext) {
  return Equal(this).shallowReduce();
}

Expression Equal::shallowReduce() {
  {
    Expression e = SimplificationHelper::shallowReduceUndefined(*this);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  if (childAtIndex(0).isIdenticalTo(childAtIndex(1))) {
    Expression result = Rational::Builder(1);
    replaceWithInPlace(result);
    return result;
  }
  return *this;
}

}
