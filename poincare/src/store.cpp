extern "C" {
#include <assert.h>
#include <stdlib.h>
#include <math.h>
}
#include <poincare/store.h>
#include <ion.h>
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

int Store::writeTextInBuffer(char * buffer, int bufferSize, PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutEngine::writeInfixExpressionTextInBuffer(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "\x90");
}

Expression * Store::shallowReduce(Context& context, AngleUnit angleUnit) {
  context.setExpressionForSymbolName(value(), symbol(), context);
  return replaceWith(editableOperand(1), true)->shallowReduce(context, angleUnit);
}

LayoutRef Store::createLayout(PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const {
  HorizontalLayoutRef result = HorizontalLayoutRef();
  result.addOrMergeChildAtIndex(value()->createLayout(floatDisplayMode, numberOfSignificantDigits), 0, false);
  result.addChildAtIndex(CharLayoutRef(Ion::Charset::Sto), result.numberOfChildren(), nullptr);
  result.addOrMergeChildAtIndex(symbol()->createLayout(floatDisplayMode, numberOfSignificantDigits), result.numberOfChildren(), false);
  return result;
}

template<typename T>
Evaluation<T> * Store::templatedApproximate(Context& context, AngleUnit angleUnit) const {
  context.setExpressionForSymbolName(value(), symbol(), context);
  if (context.expressionForSymbol(symbol()) != nullptr) {
    return context.expressionForSymbol(symbol())->privateApproximate(T(), context, angleUnit);
  }
  return new Complex<T>(Complex<T>::Undefined());
}

}
