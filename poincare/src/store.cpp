extern "C" {
#include <assert.h>
#include <stdlib.h>
#include <math.h>
}
#include <poincare/store.h>
#include <ion.h>
#include <poincare/complex.h>
#include <poincare/context.h>
#include "layout/horizontal_layout.h"
#include "layout/string_layout.h"

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
  ExpressionLayout * childrenLayouts[3];
  childrenLayouts[0] = value()->createLayout(floatDisplayMode, complexFormat);
  const char stoSymbol[2] = {Ion::Charset::Sto, 0};
  childrenLayouts[1] = new StringLayout(stoSymbol, 1);
  childrenLayouts[2] = symbol()->createLayout(floatDisplayMode, complexFormat);
  return new HorizontalLayout(childrenLayouts, 3);
}

template<typename T>
Expression * Store::templatedEvaluate(Context& context, AngleUnit angleUnit) const {
  Expression * valueEvaluation = value()->evaluate<T>(context, angleUnit);
  context.setExpressionForSymbolName(valueEvaluation, symbol(), context);
  delete valueEvaluation;
  if (context.expressionForSymbol(symbol()) != nullptr) {
    return context.expressionForSymbol(symbol())->evaluate<T>(context, angleUnit);
  }
  return new Complex<T>(Complex<T>::Float(NAN));
}

}
