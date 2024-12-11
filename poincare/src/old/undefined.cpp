#include <poincare/k_tree.h>
#include <poincare/layout.h>
#include <poincare/old/complex.h>
#include <poincare/old/symbol.h>
#include <poincare/old/undefined.h>
#include <poincare/src/memory/tree_stack.h>

#include <algorithm>

extern "C" {
#include <string.h>

#include <cmath>
}

namespace Poincare {

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

Undefined Undefined::Builder() {
  Expression expr = Expression::Builder(KUndef->cloneTree());
  return static_cast<Undefined&>(expr);
}

}  // namespace Poincare
