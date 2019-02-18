#include <poincare/ceiling_layout.h>

namespace Poincare {

CeilingLayout CeilingLayout::Builder(Layout child) {
  void * bufferNode = TreePool::sharedPool()->alloc(sizeof(CeilingLayoutNode));
  CeilingLayoutNode * node = new (bufferNode) CeilingLayoutNode();
  TreeHandle h = TreeHandle::BuildWithBasicChildren(node, &child, 1);
  return static_cast<CeilingLayout &>(h);
}

}
