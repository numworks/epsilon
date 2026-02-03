#pragma once

#include <poincare/src/memory/tree.h>

#include <complex>

namespace Poincare::Internal {

class Tree;

class Undefined {
 public:
  static bool ShallowBubbleUpUndef(Tree* e);
  static bool CanBeUndefined(const Tree* e);
  static bool CanHaveUndefinedChild(const Tree* e, int childIndex);
  /* For types which can have both defined and undefined children, i.e. lists
   * matrices and points. */
  static bool HasUndefinedDescendant(const Tree* e) {
    return e->hasDescendantSatisfying(
        [](const Tree* e) { return e->isUndefined(); });
  }
  /* Create a tree of type [type] with identical dimension to [e].
   * Example:
   * [e] is list and type is nonreal => {nonreal,...} with correct length.
   * [e] is matrix type is undef => [[undef,...]...] with correct size.
   * [e] is boolean => booleanUndef. */
  static Tree* CreateTreeWithDimensionedType(const Tree* e, Type type);
  static void ReplaceTreeWithDimensionedType(Tree* e, Type type) {
    e->moveTreeOverTree(CreateTreeWithDimensionedType(e, type));
  }
  /* Check if an undefined tree created by [ReplaceTreeWithDimensionedType] */
  static bool IsDimensionedUndefined(const Tree* t);
  template <typename T>
  static bool IsUndefined(std::complex<T> c) {
    return std::isnan(c.real()) || std::isnan(c.imag());
  };
};

}  // namespace Poincare::Internal
