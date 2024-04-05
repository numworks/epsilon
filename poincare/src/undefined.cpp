#include <poincare/complex.h>
#include <poincare/layout.h>
#include <poincare/symbol.h>
#include <poincare/undefined.h>
#include <poincare_junior/src/memory/edition_pool.h>

#include <algorithm>

extern "C" {
#include <math.h>
#include <string.h>
}

namespace Poincare {

int UndefinedNode::polynomialDegree(Context* context,
                                    const char* symbolName) const {
  /* Previously the return value was -1, but it was causing problems in the
  ¨* equations of type `y = piecewise(x,x>0,undefined,x<=0)` since the computed
   * yDeg here was -1 instead of 0. */
  return 0;
}

bool UndefinedNode::derivate(const ReductionContext& reductionContext,
                             Symbol symbol, OExpression symbolValue) {
  return true;
}

size_t UndefinedNode::serialize(char* buffer, size_t bufferSize,
                                Preferences::PrintFloatMode floatDisplayMode,
                                int numberOfSignificantDigits) const {
  return std::min<size_t>(strlcpy(buffer, Undefined::Name(), bufferSize),
                          bufferSize - 1);
}

template <typename T>
Evaluation<T> UndefinedNode::templatedApproximate() const {
  return Complex<T>::Undefined();
}

JuniorUndefined JuniorUndefined::Builder() {
  JuniorExpression expr = JuniorExpression::Builder(
      PoincareJ::SharedTreeStack->push(PoincareJ::Type::Undefined));
  return static_cast<JuniorUndefined&>(expr);
}

template Evaluation<float> UndefinedNode::templatedApproximate() const;
template Evaluation<double> UndefinedNode::templatedApproximate() const;
}  // namespace Poincare
