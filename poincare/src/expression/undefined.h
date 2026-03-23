#pragma once

#include <poincare/src/memory/tree.h>

#include <complex>

namespace Poincare::Internal {

class Tree;

class Undefined {
 public:
  /* Type order matters when bubbling up undefined children, the "biggest" one
   * is considered most important. */
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
  /* Return undefined description.
   * This is currently only used for debugging, not for user display. */
  static const char* Description(const Tree* e);
  static bool IsUndefined(const Tree* e, SpecialType type) {
    return e->isUndefSpecial() && GetSpecialType(e) == type;
  }

  static bool ShallowBubbleUpUndef(Tree* e);
  static bool CanBeUndefined(const Tree* e);
  static bool MustBubbleUpUndefinedChild(const Tree* e, int childIndex);
  /* For types which can have both defined and undefined children, i.e. lists
   * matrices and points. */
  static bool HasUndefinedDescendant(const Tree* e) {
    return e->hasDescendantSatisfying(
        [](const Tree* e) { return e->isUndefined(); });
  }
  /* Create a tree [e] with identical dimension to [target].
   * Example:
   * [target] is list and [e] is nonreal => {nonreal,...} with correct length.
   * [target] is matrix and [e] is undef => [[undef,...]...] with correct size.
   * [target] is boolean and [e] is undef => booleanUndef. */
  static Tree* CreateTreeOfTargetDimension(const Tree* e, const Tree* target);
  // Replace [e] with [replace] while preserving the dimension of [e].
  static void ReplaceTreePreservingDimension(Tree* e, const Tree* replace) {
    e->moveTreeOverTree(CreateTreeOfTargetDimension(replace, e));
  }
  // Check if an undefined tree created with [CreateTreeOfTargetDimension]
  static bool IsDimensionedUndefined(const Tree* t);
  template <typename T>
  static bool IsUndefined(std::complex<T> c) {
    return std::isnan(c.real()) || std::isnan(c.imag());
  };

 private:
  static SpecialType GetSpecialType(const Tree* e) {
    assert(e->isUndefSpecial());
    return static_cast<SpecialType>(e->nodeValue(0));
  }
};

}  // namespace Poincare::Internal
