#include "transform.h"
#include <assert.h>
#include <poincare/parenthesis.h>

bool Poincare::Simplification::ReplaceWithTransform(Expression * captures[]) {
  Expression * a = captures[0];
  Expression * b = captures[1];

  assert(a != nullptr);
  assert(a->parent() != nullptr);
  assert(b != nullptr);

  if (b->hasAncestor(a)) {
    static_cast<Hierarchy *>(b->parent())->detachOperand(b);
  }

  static_cast<Hierarchy *>(a->parent())->replaceOperand(a, const_cast<Expression *>(b));

  return true;
}
