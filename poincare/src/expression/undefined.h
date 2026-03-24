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

/* Create a tree [e] with identical dimension to [target].
 * Example:
 * [target] is list and [e] is nonreal => {nonreal,...} with correct length.
 * [target] is matrix and [e] is undef => [[undef,...]...] with correct size.
 * [target] is boolean and [e] is undef => booleanUndef. */
Tree* CreateTreeOfTargetDimension(const Tree* e, const Tree* target);

// Replace [e] with [replace] while preserving the dimension of [e].
inline void ReplaceTreePreservingDimension(Tree* e, const Tree* replace) {
  e->moveTreeOverTree(CreateTreeOfTargetDimension(replace, e));
}

// Return if t is undefined, or a list/matrix/point/sequence of undefined nodes
bool IsDimensionedUndefined(const Tree* t);

template <typename T>
inline bool IsUndefined(std::complex<T> c) {
  return std::isnan(c.real()) || std::isnan(c.imag());
};

};  // namespace Undefined

}  // namespace Poincare::Internal
