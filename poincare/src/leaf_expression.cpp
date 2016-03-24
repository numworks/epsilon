#include <poincare/leaf_expression.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

int LeafExpression::numberOfOperands() {
  return 0;
}

Expression * LeafExpression::operand(int i) {
  assert(false);
  return nullptr;
}
