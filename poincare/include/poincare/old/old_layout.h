#ifndef POINCARE_LAYOUT_REFERENCE_H
#define POINCARE_LAYOUT_REFERENCE_H

#include <escher/palette.h>
#include <omg/directions.h>

#include "context.h"
#include "layout_node.h"
#include "pool_handle.h"

namespace Poincare {

class OLayout : public PoolHandle {
 public:
  OLayout() : PoolHandle() {}
  OLayout(const LayoutNode *node) : PoolHandle(node) {}

  const LayoutNode *operator->() const {
    assert(isUninitialized() ||
           (PoolHandle::object() && !PoolHandle::object()->isGhost()));
    return static_cast<const LayoutNode *>(PoolHandle::object());
  }

  LayoutNode *operator->() {
    assert(isUninitialized() ||
           (PoolHandle::object() && !PoolHandle::object()->isGhost()));
    return static_cast<LayoutNode *>(PoolHandle::object());
  }

  bool isIdenticalTo(OLayout l, bool makeEditable = false) const {
    return isUninitialized() ? l.isUninitialized()
                             : (*this)->isIdenticalTo(l, makeEditable);
  }
};

}  // namespace Poincare

#endif
