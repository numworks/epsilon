#ifndef POINCARE_N_ARY_H
#define POINCARE_N_ARY_H

#include <poincare_junior/src/memory/edition_reference.h>

namespace PoincareJ {

class NAry {
 public:
  static void AddChild(EditionReference nary, EditionReference child) {
    return AddChildAtIndex(nary, child, nary.numberOfChildren());
  }
  static void AddChildAtIndex(EditionReference nary, EditionReference child,
                              int index);
  static void AddOrMergeChildAtIndex(EditionReference nary,
                                     EditionReference child, int index);
  static EditionReference DetachChildAtIndex(EditionReference nary, int index);
  static void RemoveChildAtIndex(EditionReference nary, int index);
  static void SetNumberOfChildren(EditionReference reference,
                                  size_t numberOfChildren);
  static EditionReference Flatten(EditionReference reference);
  static EditionReference SquashIfUnary(EditionReference reference);
  static EditionReference SquashIfEmpty(EditionReference reference);
  static EditionReference Sanitize(EditionReference reference);
  /* With comparePowBaseInMult, Power uses its base for comparison in
   * multiplications.
   * TODO: Use a true comparePowBaseInMult everywhere except right after
   *       SystemProjection to assert the expression no longer contains powers
   *       that could slip in-between two nodes of the same type (which could
   *       cause issues in most pattern matches). We may need a similar setup
   *       with Additions/Multiplications. */
  static void SortChildren(EditionReference reference,
                           bool comparePowBaseInMult = true);
};

}  // namespace PoincareJ

#endif
