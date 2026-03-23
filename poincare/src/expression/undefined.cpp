#include "undefined.h"

#include <poincare/src/memory/tree_stack.h>

#include "dimension.h"
#include "integer.h"
#include "k_tree.h"
#include "list.h"

namespace Poincare::Internal {

const char* Undefined::Description(const Tree* e) {
  assert(e && e->isUndefined());
  switch (e->type()) {
    case Type::UndefSystem:
      return "Internal undefined used by system, shouldn't be displayed";
    case Type::Undef:
      return "General Undefined";
    case Type::NonReal:
      return "nonreal";
  }
  assert(e->isUndefSpecial());
  switch (Undefined::GetSpecialType(e)) {
    case SpecialType::FailedSimplification:
      return "Raised TreeStackOverFlow or IntegerOverFlow";
    case SpecialType::ZeroPowerZero:
      return "0^0";
    case SpecialType::ZeroDivision:
      return "1/0, tan(nπ/2)";
    case SpecialType::Boolean:
      return "Undefined with Boolean dimension";
    case SpecialType::Unit:
      return "Undefined with Unit dimension";
    case SpecialType::Unhandled:
      return "inf - inf, 0 * inf, unimplemented";
    case SpecialType::UnhandledDimension:
      return "[[1,2]] + [[1],[2]]";
    case SpecialType::BadType:
      return "Non-integers in gcd,lcm,...";
    case SpecialType::OutOfDefinition:
      return "arg(0)";
    case SpecialType::NotDefined:
      return "Global variable that has not been defined";
    case SpecialType::Forbidden:
      return "Forbidden by preferences, exam-modes, ...";
    default:
      OMG_UNREACHABLE;
  }
}

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

Tree* Undefined::CreateTreeOfTargetDimension(const Tree* e,
                                             const Tree* target) {
  assert(!target->isDepList());
  Tree* result = Tree::FromBlocks(SharedTreeStack->lastBlock());
  int length = Dimension::ListLength(target);
  if (length >= 0) {
    SharedTreeStack->pushListSequence();
    "k"_e->cloneTree();
    Integer::Push(length);
  }
  Dimension dim = Dimension::Get(target);
  if (dim.isBoolean()) {
    assert(e->isUndefined());
    KUndefBoolean->cloneTree();
  } else if (dim.isUnit()) {
    assert(e->isUndefined());
    KUndefUnit->cloneTree();
  } else {
    if (IsUndefined(e, Undefined::SpecialType::Boolean) ||
        IsUndefined(e, Undefined::SpecialType::Unit)) {
      e = KUndef;
    }
    if (dim.isMatrix()) {
      int nRows = dim.matrix.rows;
      int nCols = dim.matrix.cols;
      SharedTreeStack->pushMatrix(nRows, nCols);
      for (int i = 0; i < nRows * nCols; i++) {
        e->cloneTree();
      }
    } else if (dim.isPoint()) {
      SharedTreeStack->pushPoint();
      e->cloneTree();
      e->cloneTree();
    } else {
      e->cloneTree();
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
  const Tree* worstChild = nullptr;
  Type worstType = Type::Zero;
  for (const Tree* child : e->children()) {
    // Piecewise can have undefined branches, but not conditions
    if (child->isUndefined() && MustBubbleUpUndefinedChild(e, i)) {
      Type childType = child->type();
      if (childType > worstType ||
          (childType == worstType && child->isUndefSpecial() &&
           Undefined::GetSpecialType(child) >
               Undefined::GetSpecialType(worstChild))) {
        worstType = childType;
        worstChild = child;
      }
    }
    i++;
  }
  if (worstType == Type::Zero) {
    return false;
  }
  ReplaceTreePreservingDimension(e, worstChild);
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
