#ifndef POINCARE_EXPRESSION_UNDEFINED_H
#define POINCARE_EXPRESSION_UNDEFINED_H

#include <poincare/src/memory/tree.h>

#include <complex>

namespace Poincare::Internal {

class Tree;

class Undefined {
 public:
  static bool ShallowBubbleUpUndef(Tree* e);
  static bool CanBeUndefined(const Tree* e);
  static bool CanHaveUndefinedChild(const Tree* e, int childIndex);
  /* Replace [e] with a tree of type [type] with identical dimension.
   * Example:
   * [e] is list and type is nonreal => {nonreal,...} with correct length.
   * [e] is matrix type is undef => [[undef,...]...] with correct size.
   * [e] is boolean => booleanUndef. */
  static void ReplaceTreeWithDimensionedType(Tree* e, Type type);
  template <typename T>
  static bool IsUndefined(std::complex<T> c) {
    return std::isnan(c.real()) || std::isnan(c.imag());
  };
};

}  // namespace Poincare::Internal

#endif
