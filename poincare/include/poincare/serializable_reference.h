#ifndef POINCARE_SERIALIZABLE_REFERENCE_H
#define POINCARE_SERIALIZABLE_REFERENCE_H

#include <poincare/print_float.h>
#include <poincare/tree_reference.h>

namespace Poincare {

template <typename T>
class SerializableReference : public TreeReference<T> {
public:
  using TreeReference<T>::TreeReference;

  // Serialization
  bool needsParenthesisWithParent(SerializableReference<SerializableNode> parentRef) {
    assert(this->isDefined());
    return this->typedNode()->needsParenthesisWithParent(parentRef.typedNode());
  }
  int writeTextInBuffer(char * buffer, int bufferSize, PrintFloat::Mode floatDisplayMode = PrintFloat::Mode::Decimal, int numberOfSignificantDigits = 0) const {
    assert(this->isDefined());
    return this->typedNode()->writeTextInBuffer(buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits);
  }

  // Tree
  SerializableReference<SerializableNode> serializableChildAtIndex(int i) {
    TreeReference<T> treeRefChild = TreeReference<T>::treeChildAtIndex(i);
    return SerializableReference<SerializableNode>(treeRefChild.node());
  }
};

typedef SerializableReference<SerializableNode> SerializableRef;

}

#endif
