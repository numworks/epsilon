#include "transform.h"
#include <assert.h>
#include <poincare/integer.h>
#include <poincare/dynamic_hierarchy.h>
#include <poincare/addition.h>
#include <poincare/multiplication.h>
#include <utility>

namespace Poincare {

typedef Integer (*Operation)(const Integer&, const Integer &);

static bool IntegerDynamicHierarchyTransform(Expression * captures[], Operation operation) {
  DynamicHierarchy * a = static_cast<DynamicHierarchy *>(captures[0]);
  Integer * i1 = static_cast<Integer *>(captures[1]);
  Integer * i2 = static_cast<Integer *>(captures[2]);

  Integer resultOnStack = operation(*i1, *i2);
  Integer * r = new Integer(std::move(resultOnStack));

  a->replaceOperand(i1, r);
  a->removeOperand(i2);
  return true;
}

bool Simplification::IntegerAdditionTransform(Expression * captures[]) {
  return IntegerDynamicHierarchyTransform(captures, Integer::Addition);
}

bool Simplification::IntegerMultiplicationTransform(Expression * captures[]) {
  return IntegerDynamicHierarchyTransform(captures, Integer::Multiplication);
}

}
