#include <poincare/modulo.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/floor.h>
#include <poincare/multiplication.h>
#include <poincare/subtraction.h>
#include <poincare/division.h>
#include <assert.h>

namespace Poincare {


// Simplication

Expression ModuloNode::shallowReduce(ReductionContext reductionContext) {
  return Modulo(this).shallowReduce(reductionContext);
}


// Layout
Layout ModuloNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Infix(Modulo(this), floatDisplayMode, numberOfSignificantDigits, "%");
}

int ModuloNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Infix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "%");
}

template<typename T> 
Complex<T> ModuloNode::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  return Complex<T>::Builder(Modulo(this).approximateToScalar<T>(context, complexFormat, angleUnit));
}

Expression Modulo::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  assert(numberOfChildren() > 2);
  Expression a = childAtIndex(0);
  Expression b = childAtIndex(1);
  return Subtraction::Builder(a,
    Multiplication::Builder(b,
      Floor::Builder(
        Division::Builder(a,b)).clone())); 
}

}
