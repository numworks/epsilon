#include "simplify_addition_merge.h"
#include <poincare/addition.h>
extern "C" {
#include <stdlib.h>
#include <assert.h>
}

Expression * SimplifyAdditionMerge(Expression * e) {
  if (e->type() != Expression::Type::Addition) {
    return nullptr;
  }
  Addition * a = (Addition *)e;
  // Let's count how many elements we'll be able to pick up from our children
  int mergedNumberOfChildren = 0;
  for (int i=0; i<a->numberOfOperands(); i++) {
    Expression * operand = a->operand(i);
    if (operand->type() == Expression::Type::Addition) {
      mergedNumberOfChildren += operand->numberOfOperands();
    } else {
      mergedNumberOfChildren += 1;
    }
  }
  assert(mergedNumberOfChildren >= a->numberOfOperands());
  if (mergedNumberOfChildren == a->numberOfOperands()) {
    // Nothing to simplify here
    return nullptr;
  }
  Expression ** children = (Expression **)malloc(sizeof(Expression *) * mergedNumberOfChildren);
  int childrenIndex = 0;
  for (int i=0; i<a->numberOfOperands(); i++) {
    Expression * operand = a->operand(i);
    if (operand->type() == Expression::Type::Addition) {
      for (int j=0; j<operand->numberOfOperands(); j++) {
        children[childrenIndex++] = operand->operand(j);
      }
    } else {
      children[childrenIndex++] = operand;
    }
  }
  Addition * result = new Addition(children, mergedNumberOfChildren, true);
  free(children);
  return result;
}
