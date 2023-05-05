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
};

}  // namespace PoincareJ

#endif
