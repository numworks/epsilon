#include "undefined.h"

#include <poincare/src/memory/tree_stack.h>

#include "dimension.h"
#include "integer.h"
#include "k_tree.h"
#include "list.h"

namespace Poincare::Internal {

#if POINCARE_TREE_LOG

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
      return "Raised TreeStackOverflow or IntegerOverflow";
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

#endif

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

// Return true if [e] is a strictly more important undefined node than [ref]
bool IsMoreImportant(const Tree* e, const Tree* ref) {
  assert(e->isUndefined() && ref->isUndefined());
  return e->type() > ref->type() ||
         (e->isUndefSpecial() && ref->isUndefSpecial() &&
          Undefined::GetSpecialType(e) > Undefined::GetSpecialType(ref));
}

const Tree* Undefined::GetUndefinedChild(const Tree* e) {
  const Tree* worstChild = nullptr;
  for (const Tree* child : e->children()) {
    if (child->isUndefined() &&
        (!worstChild || IsMoreImportant(child, worstChild))) {
      worstChild = child;
    }
  }
  return worstChild;
}

Tree* Undefined::CreateDimensionedUndef(const Tree* undefTree,
                                        const Tree* targetForDimension) {
  assert(!targetForDimension->isDepList());
  assert(undefTree->isUndefined());
  Tree* result = Tree::FromBlocks(SharedTreeStack->lastBlock());
  int length = Dimension::ListLength(targetForDimension);
  if (length >= 0) {
    SharedTreeStack->pushListSequence();
    "k"_e->cloneTree();
    Integer::Push(length);
  }
  Dimension dim = Dimension::Get(targetForDimension);
  if (dim.isBoolean()) {
    KUndefBoolean->cloneTree();
  } else if (dim.isUnit()) {
    KUndefUnit->cloneTree();
  } else {
    if (IsUndefSpecial(undefTree, Undefined::SpecialType::Boolean) ||
        IsUndefSpecial(undefTree, Undefined::SpecialType::Unit)) {
      undefTree = KUndef;
    }
    if (dim.isMatrix()) {
      int nRows = dim.matrix.rows;
      int nCols = dim.matrix.cols;
      SharedTreeStack->pushMatrix(nRows, nCols);
      for (int i = 0; i < nRows * nCols; i++) {
        undefTree->cloneTree();
      }
    } else if (dim.isPoint()) {
      SharedTreeStack->pushPoint();
      undefTree->cloneTree();
      undefTree->cloneTree();
    } else {
      undefTree->cloneTree();
    }
  }
  if (length >= 0) {
    // Expand list sequence to list
    List::BubbleUp(result);
  }
  return result;
}

bool Undefined::ShallowBubbleUpUndef(Tree* e) {
  const Tree* worstChild = nullptr;
  for (IndexedChild<const Tree*> child : e->indexedChildren()) {
    // Piecewise can have undefined branches, but not conditions
    if (child->isUndefined() && MustBubbleUpUndefinedChild(e, child.index) &&
        (!worstChild || IsMoreImportant(child, worstChild))) {
      worstChild = child;
    }
  }
  if (!worstChild) {
    return false;
  }
  ReplaceTreePreservingDimension(e, worstChild);
  return true;
}

static bool CheckUndefinedOrNonReal(TypeBlock type, bool checkNonReal) {
  if (checkNonReal) {
    return type.isNonReal();
  }
  return type.isUndefined();
}

static bool CheckUndefinedOrNonReal(const Tree* t, bool checkNonReal) {
  return CheckUndefinedOrNonReal(t->type(), checkNonReal);
}

static bool IsDimensionedUndefinedOrNonReal(const Tree* t, bool checkNonReal) {
  if (CheckUndefinedOrNonReal(t, checkNonReal)) {
    return true;
  }
  if (t->isMatrix() || t->isList() || t->isPoint()) {
    if (t->numberOfChildren() == 0) {
      return false;
    }
    // Check all children are the same and undefined
    const Tree* child = t->child(0);
    TypeBlock type = child->type();
    if (!CheckUndefinedOrNonReal(type, checkNonReal)) {
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
    return CheckUndefinedOrNonReal(t->child(Parametric::FunctionIndex(t)),
                                   checkNonReal);
  }
  return false;
}

bool Undefined::IsDimensionedUndefined(const Tree* t) {
  return IsDimensionedUndefinedOrNonReal(t, false);
}

bool Undefined::IsDimensionedNonReal(const Tree* t) {
  return IsDimensionedUndefinedOrNonReal(t, true);
}

}  // namespace Poincare::Internal
