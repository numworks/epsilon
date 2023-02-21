#include <poincare/complex.h>
#include <poincare/layout_helper.h>
#include <poincare/symbol.h>
#include <poincare/undefined.h>

#include <algorithm>

extern "C" {
#include <math.h>
#include <string.h>
}

namespace Poincare {

int UndefinedNode::polynomialDegree(Context* context,
                                    const char* symbolName) const {
  return -1;
}

Layout UndefinedNode::createLayout(Preferences::PrintFloatMode floatDisplayMode,
                                   int numberOfSignificantDigits,
                                   Context* context) const {
  return LayoutHelper::String(Undefined::Name(), Undefined::NameSize() - 1);
}

bool UndefinedNode::derivate(const ReductionContext& reductionContext,
                             Symbol symbol, Expression symbolValue) {
  return true;
}

int UndefinedNode::serialize(char* buffer, int bufferSize,
                             Preferences::PrintFloatMode floatDisplayMode,
                             int numberOfSignificantDigits) const {
  return std::min<int>(strlcpy(buffer, Undefined::Name(), bufferSize),
                       bufferSize - 1);
}

template <typename T>
Evaluation<T> UndefinedNode::templatedApproximate() const {
  return Complex<T>::Undefined();
}

template Evaluation<float> UndefinedNode::templatedApproximate() const;
template Evaluation<double> UndefinedNode::templatedApproximate() const;
}  // namespace Poincare
