extern "C" {
#include <assert.h>
#include <stdlib.h>
#include <math.h>
}
#include <poincare/store.h>
#include <ion.h>
#include <poincare/complex.h>
#include <poincare/context.h>
#include <poincare/char_layout_node.h>
#include <poincare/horizontal_layout_node.h>


namespace Poincare {

Expression::Type Store::type() const {
  return Type::Store;
}

Expression * Store::clone() const {
  return new Store(operands(), true);
}

int Store::polynomialDegree(char symbolName) const {
  return -1;
}

static_assert('\x90' == Ion::Charset::Sto, "Incorrect");
int Store::writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits) const {
  return LayoutEngine::writeInfixExpressionTextInBuffer(this, buffer, bufferSize, numberOfSignificantDigits, "\x90");
}

Expression * Store::shallowReduce(Context& context, AngleUnit angleUnit) {
  context.setExpressionForSymbolName(value(), symbol(), context);
  return replaceWith(editableOperand(1), true)->shallowReduce(context, angleUnit);
}

LayoutRef Store::privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != PrintFloat::Mode::Default);
  assert(complexFormat != ComplexFormat::Default);
  HorizontalLayoutRef result = HorizontalLayoutRef();
  result.addOrMergeChildAtIndex(value()->createLayout(floatDisplayMode, complexFormat), 0, false);
  result.addChildAtIndex(CharLayoutRef(Ion::Charset::Sto), result.numberOfChildren(), nullptr);
  result.addOrMergeChildAtIndex(symbol()->createLayout(floatDisplayMode, complexFormat), result.numberOfChildren(), false);
  return result;
}

template<typename T>
Expression * Store::templatedApproximate(Context& context, AngleUnit angleUnit) const {
  context.setExpressionForSymbolName(value(), symbol(), context);
  if (context.expressionForSymbol(symbol()) != nullptr) {
    return context.expressionForSymbol(symbol())->approximate<T>(context, angleUnit);
  }
  return new Complex<T>(Complex<T>::Float(NAN));
}

}
