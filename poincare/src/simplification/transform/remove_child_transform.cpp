#include "transform.h"
#include <assert.h>
#include <poincare/dynamic_hierarchy.h>

bool Poincare::Simplification::RemoveChildTransform(Expression * captures[]) {
  Expression * a = captures[0];
  Expression * b = captures[1];

  assert(a != nullptr);
  assert(b != nullptr);

  static_cast<DynamicHierarchy *>(a)->removeOperand(b, true);

  return true;
}
