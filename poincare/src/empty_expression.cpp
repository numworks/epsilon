#include <poincare/empty_expression.h>
#include <poincare/empty_layout_node.h>
#include <poincare/layout_engine.h>
#include <ion/charset.h>
#include <math.h>

namespace Poincare {

Expression * EmptyExpression::clone() const {
  return new EmptyExpression();
}

int EmptyExpression::writeTextInBuffer(char * buffer, int bufferSize, PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutEngine::writeOneCharInBuffer(buffer, bufferSize, Ion::Charset::Empty);
}

LayoutRef EmptyExpression::createLayout(PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const {
  return EmptyLayoutRef();
}

template<typename T> Complex<T> * EmptyExpression::templatedApproximate(Context& context, AngleUnit angleUnit) const {
  return new Complex<T>(Complex<T>::Undefined());
}

}
