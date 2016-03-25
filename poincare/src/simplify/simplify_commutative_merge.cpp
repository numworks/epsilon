#include "simplify_commutative_merge.h"
#include <poincare/addition.h>
#include <poincare/product.h>
extern "C" {
#include <stdlib.h>
#include <assert.h>
}

Expression * SimplifyCommutativeMerge(Expression * e) {
  if (e->type() != Expression::Type::Addition &&
      e->type() != Expression::Type::Product) {
    return nullptr;
  }
  CommutativeOperation * c = (CommutativeOperation *)e;
  // Let's count how many elements we'll be able to pick up from our children
  int mergedNumberOfChildren = 0;
  for (int i=0; i<c->numberOfOperands(); i++) {
    Expression * operand = c->operand(i);
    if (operand->type() == c->type()) {
      mergedNumberOfChildren += operand->numberOfOperands();
    } else {
      mergedNumberOfChildren += 1;
    }
  }
  assert(mergedNumberOfChildren >= c->numberOfOperands());
  if (mergedNumberOfChildren == c->numberOfOperands()) {
    // Nothing to simplify here
    return nullptr;
  }
  Expression ** children = (Expression **)malloc(sizeof(Expression *) * mergedNumberOfChildren);
  int childrenIndex = 0;
  for (int i=0; i<c->numberOfOperands(); i++) {
    Expression * operand = c->operand(i);
    if (operand->type() == c->type()) {
      for (int j=0; j<operand->numberOfOperands(); j++) {
        children[childrenIndex++] = operand->operand(j);
      }
    } else {
      children[childrenIndex++] = operand;
    }
  }
  CommutativeOperation * result = nullptr;
  switch(e->type()) {
    case Expression::Type::Addition:
      result = new Addition(children, mergedNumberOfChildren, true);
      break;
    case Expression::Type::Product:
      result = new Product(children, mergedNumberOfChildren, true);
      break;
    default:
      assert(false);
  }
  free(children);
  return result;
}
