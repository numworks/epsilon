#include "undefined.h"

#include <poincare/src/memory/tree_stack.h>

#include "dimension.h"
#include "integer.h"
#include "k_tree.h"
#include "list.h"

namespace Poincare::Internal {

bool Undefined::CanBeUndefined(const Tree* e) {
  if (e->isPoint() || e->isList() || e->isMatrix() || e->isDepList()) {
    // All their children can be undefined and it is never bubbled up.
    return false;
  }
  if (e->numberOfChildren() == 0) {
    // Undefined trees are the only childless trees that can be undefined.
    /* Note: Variables and UserVariables are considered to always be defined to
     * allow better reductions. For now, Variables and UserVariables that
     * haven't been replaced by their definition are always evaluated on defined
     * values. */
    return e->isUndefined();
  }
  // By default, any other tree can be undefined. For ex: ln(0) or abs(undef).
  return true;
}

bool IsPiecewiseBranchIndex(int childIndex, const Tree* parent) {
  return parent->isPiecewise() && childIndex % 2 == 0;
}

bool IsListSequenceFunctionIndex(int childIndex, const Tree* parent) {
  return parent->isListSequence() &&
         Parametric::IsFunctionIndex(childIndex, parent);
}
bool Undefined::MustBubbleUpUndefinedChild(const Tree* e, int childIndex) {
  return CanBeUndefined(e) && !IsPiecewiseBranchIndex(childIndex, e) &&
         !IsListSequenceFunctionIndex(childIndex, e);
}

Tree* Undefined::CreateTreeWithDimensionedType(const Tree* e, Type type) {
  assert(TypeBlock::IsZero(type) || TypeBlock::IsUndefined(type));
  assert(!e->isDepList());
  Tree* result = Tree::FromBlocks(SharedTreeStack->lastBlock());
  int length = Dimension::ListLength(e);
  if (length >= 0) {
    SharedTreeStack->pushListSequence();
    "k"_e->cloneTree();
    Integer::Push(length);
  }
  Dimension dim = Dimension::Get(e);
  if (dim.isBoolean()) {
    assert(TypeBlock::IsUndefined(type));
    SharedTreeStack->pushBlock(Type::UndefBoolean);
  } else if (dim.isUnit()) {
    assert(TypeBlock::IsUndefined(type));
    SharedTreeStack->pushBlock(Type::UndefUnit);
  } else {
    if (type == Type::UndefBoolean || type == Type::UndefUnit) {
      type = Type::Undef;
    }
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
  }
  if (length >= 0) {
    // Expand list sequence to list
    List::BubbleUp(result);
  }
  return result;
}

bool Undefined::ShallowBubbleUpUndef(Tree* e) {
  uint8_t i = 0;
  Type worstType = Type::Zero;
  for (const Tree* child : e->children()) {
    // Piecewise can have undefined branches, but not conditions
    if (child->isUndefined() && MustBubbleUpUndefinedChild(e, i)) {
      Type childType = child->type();
      worstType = childType > worstType ? childType : worstType;
    }
    i++;
  }
  if (worstType == Type::Zero) {
    return false;
  }
  ReplaceTreeWithDimensionedType(e, worstType);
  return true;
}

bool Undefined::IsDimensionedUndefined(const Tree* t) {
  if (t->isUndefined()) {
    return true;
  }
  if (t->isMatrix() || t->isList() || t->isPoint()) {
    if (t->numberOfChildren() == 0) {
      return false;
    }
    // Check all children are the same and undefined
    const Tree* child = t->child(0);
    TypeBlock type = child->type();
    if (!type.isUndefined()) {
      return false;
    }
    for (const Tree* child : t->children()) {
      if (child->type() != type) {
        return false;
      }
    }
    return true;
  }
  if (t->isListSequence()) {
    return t->child(Parametric::FunctionIndex(t))->isUndefined();
  }
  return false;
}
}  // namespace Poincare::Internal
