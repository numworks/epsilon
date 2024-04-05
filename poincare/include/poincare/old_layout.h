#ifndef POINCARE_LAYOUT_REFERENCE_H
#define POINCARE_LAYOUT_REFERENCE_H

#include <escher/palette.h>
#include <omg/directions.h>
#include <poincare/context.h>
#include <poincare/layout_node.h>
#include <poincare/tree_handle.h>
#include <poincare/trinary_boolean.h>

namespace Poincare {

class OLayout : public PoolHandle {
 public:
  OLayout() : PoolHandle() {}
  OLayout(const LayoutNode *node) : PoolHandle(node) {}
  OLayout clone() const;

  const LayoutNode *operator->() const {
    assert(isUninitialized() ||
           (PoolHandle::node() && !PoolHandle::node()->isGhost()));
    return static_cast<const LayoutNode *>(PoolHandle::node());
  }

  LayoutNode *operator->() {
    assert(isUninitialized() ||
           (PoolHandle::node() && !PoolHandle::node()->isGhost()));
    return static_cast<LayoutNode *>(PoolHandle::node());
  }

  bool isIdenticalTo(OLayout l, bool makeEditable = false) const {
    return isUninitialized() ? l.isUninitialized()
                             : (*this)->isIdenticalTo(l, makeEditable);
  }

  // Serialization
  size_t serializeForParsing(char *buffer, size_t bufferSize) const {
    return (*this)->serialize(buffer, bufferSize);
  }
  size_t serializeParsedExpression(char *buffer, size_t bufferSize,
                                   Context *context) const;
};

}  // namespace Poincare

#endif
