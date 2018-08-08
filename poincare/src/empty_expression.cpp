#include <poincare/empty_expression.h>
#include <poincare/empty_layout_node.h>
#include <poincare/layout_helper.h>
#include <ion/charset.h>
#include <math.h>

namespace Poincare {

Expression * EmptyExpression::clone() const {
  return new EmptyExpression();
}

int EmptyExpression::writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Char(buffer, bufferSize, Ion::Charset::Empty);
}

LayoutRef EmptyExpression::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return EmptyLayoutRef();
}

template<typename T> Complex<T> * EmptyExpression::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  return new Complex<T>(Complex<T>::Undefined());
}

}
