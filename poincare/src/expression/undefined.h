#pragma once

#include <poincare/src/memory/tree.h>

#include <complex>

namespace Poincare::Internal {

class Tree;

namespace Undefined {
/* Type order matters when bubbling up undefined children, the "biggest" one is
 * considered most important. */
enum class SpecialType : uint8_t {
  FailedSimplification = 0,
  ZeroPowerZero,
  ZeroDivision,
  Boolean,
  Unit,
  Unhandled,
  UnhandledDimension,
  BadType,
  OutOfDefinition,
  NotDefined,
  Forbidden,
};

#if POINCARE_TREE_LOG
/* Return undefined description.
 * This is currently only used for debugging, not for user display. */
const char* Description(const Tree* e);
#endif

inline SpecialType GetSpecialType(const Tree* e) {
  assert(e->isUndefSpecial());
  return static_cast<SpecialType>(e->nodeValue(0));
}

inline bool IsUndefSpecial(const Tree* e, SpecialType type) {
  return e->isUndefSpecial() && GetSpecialType(e) == type;
}

bool ShallowBubbleUpUndef(Tree* e);

bool CanBeUndefined(const Tree* e);

bool MustBubbleUpUndefinedChild(const Tree* e, int childIndex);

/* Return the first undefined child, nullptr if there are none. */
const Tree* GetUndefinedChild(const Tree* e);

/* Create a tree [undefTree] with identical dimension to [targetForDimension].
 * Example:
 * [targetForDimension] is list and [undefTree] is nonreal
 *                     => {nonreal,...} with correct length.
 * [targetForDimension] is matrix and [undefTree] is undef
 *                     => [[undef,...]...] with correct size.
 * [targetForDimension] is boolean and [undefTree] is undef
 *                     => booleanUndef. */
Tree* CreateDimensionedUndef(const Tree* undefTree,
                             const Tree* targetForDimension);

// Replace [e] with [undefTree] while preserving the dimension of [e].
inline void ReplaceTreePreservingDimension(Tree* e, const Tree* undefTree) {
  e->moveTreeOverTree(CreateDimensionedUndef(undefTree, e));
}

// Return true if t is a scalar/list/matrix/point/sequence of undefined tree
bool IsDimensionedUndefined(const Tree* t);

template <typename T>
inline bool IsUndefined(std::complex<T> c) {
  return std::isnan(c.real()) || std::isnan(c.imag());
};

};  // namespace Undefined

}  // namespace Poincare::Internal
