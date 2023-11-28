#include <assert.h>
#include <poincare/junior_layout.h>

#include <algorithm>

namespace Poincare {

JuniorLayout JuniorLayout::Builder(const PoincareJ::Tree* tree) {
  size_t size = tree->treeSize();
  void* bufferNode =
      TreePool::sharedPool->alloc(sizeof(JuniorLayoutNode) + size);
  JuniorLayoutNode* node = new (bufferNode) JuniorLayoutNode(tree, size);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  return static_cast<JuniorLayout&>(h);
}

}  // namespace Poincare
