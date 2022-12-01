#include "n_ary.h"
#include <poincare_junior/src/memory/node_iterator.h>

namespace Poincare {

void NAry::SetNumberOfChildren(EditionReference reference, size_t numberOfChildren) {
  assert(numberOfChildren < UINT8_MAX);
  Block * numberOfChildrenBlock = reference.node().block()->next();
  *numberOfChildrenBlock = numberOfChildren;
}

EditionReference NAry::Flatten(EditionReference reference) {
  size_t numberOfChildren = 0;
  for (std::pair<EditionReference, int> indexedRef : NodeIterator::Children<Forward, Editable>(reference)) {
    if (reference.node().type() == std::get<EditionReference>(indexedRef).node().type()) {
      EditionReference nAry = std::get<EditionReference>(indexedRef);
      std::get<EditionReference>(indexedRef).removeNode();
    }
    numberOfChildren++;
  }
  SetNumberOfChildren(reference, numberOfChildren);
  return reference;
}

}
