#ifndef POINCARE_SERIALIZABLE_REFERENCE_H
#define POINCARE_SERIALIZABLE_REFERENCE_H

#include <poincare/print_float.h>
#include <poincare/tree_reference.h>

namespace Poincare {

class SerializableReference : public TreeReference {
public:
  using TreeReference::TreeReference;

  // Serialization
  bool needsParenthesisWithParent(SerializableReference parentRef) {
    assert(isDefined());
    return static_cast<SerializableNode *>(node())->needsParenthesisWithParent(static_cast<SerializableNode *>(parentRef.node()));
  }
  int writeTextInBuffer(char * buffer, int bufferSize, PrintFloat::Mode floatDisplayMode = PrintFloat::Mode::Decimal, int numberOfSignificantDigits = 0) const {
    assert(isDefined());
    return static_cast<SerializableNode *>(node())->writeTextInBuffer(buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits);
  }

  // Tree
  SerializableReference serializableChildAtIndex(int i) {
    TreeReference treeRefChild = TreeReference::treeChildAtIndex(i);
    return SerializableReference(treeRefChild.node());
  }
};

typedef SerializableReference SerializableRef;

}

#endif
