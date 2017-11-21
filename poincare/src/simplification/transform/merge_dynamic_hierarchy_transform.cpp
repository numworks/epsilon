#include "transform.h"
#include <assert.h>
#include <poincare/dynamic_hierarchy.h>

bool Poincare::Simplification::MergeDynamicHierarchyTransform(Expression * captures[]) {
  DynamicHierarchy * h0 = static_cast<DynamicHierarchy *>(captures[0]);
  DynamicHierarchy * h1 = static_cast<DynamicHierarchy *>(captures[1]);
  // TODO: because removeOperand squashes the hierarchy when it has only 1 operand, we have to add operands before removing them. However, it can be non-optimal as we alloc an extra space for an expressions pointer which is deleted right after...
  h0->addOperands(h1->operands(), h1->numberOfOperands());
  h0->removeOperand(h1, false);
  h1->detachOperands();
  delete h1;
  return true;
}
