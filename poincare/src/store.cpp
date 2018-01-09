extern "C" {
#include <assert.h>
#include <stdlib.h>
#include <math.h>
}
#include <poincare/store.h>
#include <ion.h>
#include <poincare/complex.h>
#include <poincare/context.h>
#include "layout/char_layout.h"
#include "layout/horizontal_layout.h"

namespace Poincare {

Expression::Type Store::type() const {
  return Type::Store;
}

Expression * Store::clone() const {
  return new Store(operands(), true);
}

static_assert('\x8F' == Ion::Charset::Sto, "Incorrect");
int Store::writeTextInBuffer(char * buffer, int bufferSize) const {
  return LayoutEngine::writeInfixExpressionTextInBuffer(this, buffer, bufferSize, "\x8F");
}

ExpressionLayout * Store::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  HorizontalLayout * result = new HorizontalLayout();
  result->addOrMergeChildAtIndex(value()->createLayout(floatDisplayMode, complexFormat), 0, false);
  result->addChildAtIndex(new CharLayout(Ion::Charset::Sto), result->numberOfChildren());
  result->addOrMergeChildAtIndex(symbol()->createLayout(floatDisplayMode, complexFormat), result->numberOfChildren(), false);
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
