#include <poincare/floor_layout.h>

namespace Poincare {

FloorLayout FloorLayout::Builder(Layout child) {
  void * bufferNode = TreePool::sharedPool()->alloc(sizeof(FloorLayoutNode));
  FloorLayoutNode * node = new (bufferNode) FloorLayoutNode();
  TreeHandle h = TreeHandle::BuildWithBasicChildren(node, &child, 1);
  return static_cast<FloorLayout &>(h);
}

}
