#include "undefined.h"

#include <poincare/src/memory/tree.h>
#include <poincare/src/memory/tree_stack.h>

#include "dimension.h"
#include "integer.h"
#include "k_tree.h"

namespace Poincare::Internal {

bool Undefined::CanBeUndefined(const Tree* e) {
  // Their elements can be undefined and it is never bubbled up.
  return !(e->isPoint() || e->isList() || e->isMatrix() ||
           e->isUnitConversion() || e->isUserSymbol() ||
           e->numberOfChildren() == 0);
}

bool Undefined::CanHaveUndefinedChild(const Tree* e, int childIndex) {
  return !CanBeUndefined(e) || (e->isPiecewise() && childIndex % 2 == 0) ||
         (e->isListSequence() && childIndex == 2);
}

void replaceTreeWithDimensionedType(Tree* e, Type type) {
  assert(TypeBlock::IsZero(type) || TypeBlock::IsUndefined(type));
  Tree* result = Tree::FromBlocks(SharedTreeStack->lastBlock());
  int length = Dimension::ListLength(e);
  if (length >= 0) {
    // Push ListSequence instead of a list to delay its expansion.
    SharedTreeStack->pushListSequence();
    KVarK->cloneTree();
    Integer::Push(length);
  }
  Dimension dim = Dimension::Get(e);
  if (dim.isMatrix()) {
    int nRows = dim.matrix.rows;
    int nCols = dim.matrix.cols;
    SharedTreeStack->pushMatrix(nRows, nCols);
    for (int i = 0; i < nRows * nCols; i++) {
      SharedTreeStack->pushBlock(type);
    }
  } else if (dim.isPoint()) {
    SharedTreeStack->pushPoint();
    SharedTreeStack->pushBlock(type);
    SharedTreeStack->pushBlock(type);
  } else {
    SharedTreeStack->pushBlock(type);
  }
  e->moveTreeOverTree(result);
}

bool Undefined::ShallowBubbleUpUndef(Tree* e) {
  uint8_t i = 0;
  Type worstType = Type::Zero;
  for (const Tree* child : e->children()) {
    // Piecewise can have undefined branches, but not conditions
    if (child->isUndefined() && !CanHaveUndefinedChild(e, i)) {
      Type childType = child->type();
      worstType = childType > worstType ? childType : worstType;
    }
    i++;
  }
  if (worstType == Type::Zero) {
    return false;
  }
  replaceTreeWithDimensionedType(e, worstType);
  return true;
}

}  // namespace Poincare::Internal
