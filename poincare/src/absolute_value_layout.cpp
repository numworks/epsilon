#include <poincare/absolute_value_layout.h>

namespace Poincare {

AbsoluteValueLayout AbsoluteValueLayout::Builder(Layout child) {
  void * bufferNode = TreePool::sharedPool()->alloc(sizeof(AbsoluteValueLayoutNode));
  AbsoluteValueLayoutNode * node = new (bufferNode) AbsoluteValueLayoutNode();
  TreeHandle h = TreeHandle::BuildWithBasicChildren(node, &child, 1);
  return static_cast<AbsoluteValueLayout &>(h);
}

}
