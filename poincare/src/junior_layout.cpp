#include <assert.h>
#include <poincare/junior_layout.h>
#include <poincare_junior/include/layout.h>

#include <algorithm>

namespace Poincare {

Layout JuniorLayoutNode::makeEditable() {
  return PoincareJ::Layout::ToPoincareLayout(tree());
}

JuniorLayout JuniorLayout::Builder(const PoincareJ::Tree* tree) {
  size_t size = tree->treeSize();
  void* bufferNode =
      TreePool::sharedPool->alloc(sizeof(JuniorLayoutNode) + size);
  JuniorLayoutNode* node = new (bufferNode) JuniorLayoutNode(tree, size);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  return static_cast<JuniorLayout&>(h);
}

JuniorLayout JuniorLayout::Juniorize(Layout l) {
  if (l.type() == LayoutNode::Type::JuniorLayout) {
    return static_cast<JuniorLayout&>(l);
  }
  PoincareJ::Tree* tree = PoincareJ::Layout::FromPoincareLayout(l);
  JuniorLayout j = Builder(tree);
  tree->removeTree();
  return j;
}

}  // namespace Poincare
