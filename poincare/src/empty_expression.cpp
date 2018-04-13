#include <poincare/empty_expression.h>
#include <poincare/layout_engine.h>
#include <poincare/src/layout/empty_layout.h>
#include <ion/charset.h>

extern "C" {
#include <math.h>
}

namespace Poincare {

Expression * EmptyExpression::clone() const {
  return new EmptyExpression();
}

int EmptyExpression::writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits) const {
  return LayoutEngine::writeOneCharInBuffer(buffer, bufferSize, Ion::Charset::Empty);
}

ExpressionLayout * EmptyExpression::privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const {
  return new EmptyLayout();
}

template<typename T> Complex<T> * EmptyExpression::templatedApproximate(Context& context, AngleUnit angleUnit) const {
  return new Complex<T>(Complex<T>::Undefined());
}

}
