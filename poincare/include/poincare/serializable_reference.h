#ifndef POINCARE_SERIALIZABLE_REFERENCE_H
#define POINCARE_SERIALIZABLE_REFERENCE_H

#include <poincare/preferences.h>
#include <poincare/tree_by_reference.h>
#include <poincare/serializable_node.h>

namespace Poincare {

class SerializableReference : virtual public TreeByReference {
public:
  SerializableNode * node() const override { return static_cast<SerializableNode *>(TreeByReference::node()); }
  // Serialization
  bool needsParenthesisWithParent(SerializableReference parentRef) {
    assert(isDefined());
    return node()->needsParenthesisWithParent(parentRef.node());
  }
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const {
    assert(isDefined());
    return node()->writeTextInBuffer(buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits);
  }

  // Tree
  SerializableReference serializableChildAtIndex(int i) {
    TreeByReference treeRefChild = TreeByReference::treeChildAtIndex(i);
    return SerializableReference(static_cast<SerializableNode *>(treeRefChild.node()));
  }
protected:
  SerializableReference(SerializableNode * n) : TreeByReference(n) {}
};

}

#endif
