#include <poincare/leaf_expression.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

bool LeafExpression::hasValidNumberOfArguments() const {
  return true;
}

int LeafExpression::numberOfOperands() const {
  return 0;
}

const Expression * LeafExpression::operand(int i) const {
  assert(false);
  return nullptr;
}

Expression * LeafExpression::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 0);
  return this->clone();
}

void LeafExpression::sort() {
}

bool LeafExpression::nodeEquals(const Expression * e) const {
  if (!Expression::nodeEquals(e)) {
    return valueEquals(e);
  }
  return false;
}

bool LeafExpression::nodeGreaterThan(const Expression * e) const {
  if (Expression::nodeEquals(e)) {
    return valueGreaterThan(e);
  }
  return Expression::nodeGreaterThan(e);
}

}
