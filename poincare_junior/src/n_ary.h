#ifndef POINCARE_N_ARY_H
#define POINCARE_N_ARY_H

#include <poincare_junior/src/memory/edition_reference.h>

namespace Poincare {

class NAry {
public:
  static void AddChild(EditionReference nary, EditionReference child);
  static void SetNumberOfChildren(EditionReference reference, size_t numberOfChildren);
  static EditionReference Flatten(EditionReference reference);
};

}

#endif
