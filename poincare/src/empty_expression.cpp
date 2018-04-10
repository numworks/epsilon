#include <poincare/empty_expression.h>
#include <poincare/layout_engine.h>
#include <poincare/src/layout/empty_visible_layout.h>
#include <ion/charset.h>

extern "C" {
#include <math.h>
}

namespace Poincare {

Expression * EmptyExpression::clone() const {
  return new EmptyExpression();
}

int EmptyExpression::writeTextInBuffer(char * buffer, int bufferSize) const {
  return LayoutEngine::writeOneCharInBuffer(buffer, bufferSize, Ion::Charset::Empty);
}

ExpressionLayout * EmptyExpression::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  return new EmptyVisibleLayout();
}

template<typename T> Complex<T> * EmptyExpression::templatedApproximate(Context& context, AngleUnit angleUnit) const {
  return new Complex<T>(Complex<T>::Float(NAN));
}

}
