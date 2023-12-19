#include <assert.h>
#include <poincare/junior_layout.h>
#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/layout/render.h>

#include <algorithm>

namespace Poincare {

KDSize JuniorLayoutNode::computeSize(KDFont::Size font) {
  return PoincareJ::Render::Size(tree());
}

KDCoordinate JuniorLayoutNode::computeBaseline(KDFont::Size font) {
  return PoincareJ::Render::Baseline(tree());
}

void JuniorLayoutNode::render(KDContext* ctx, KDPoint p, KDGlyph::Style style) {
  PoincareJ::Render::Draw(tree(), ctx, p, style.font, KDColorBlue,
                          style.backgroundColor);
}

size_t JuniorLayoutNode::serialize(char* buffer, size_t bufferSize,
                                   Preferences::PrintFloatMode floatDisplayMode,
                                   int numberOfSignificantDigits) const {
  Layout l = PoincareJ::Layout::ToPoincareLayout(tree());
  return l.node()->serialize(buffer, bufferSize, floatDisplayMode,
                             numberOfSignificantDigits);
}

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
  if (l.isUninitialized()) {
    return static_cast<JuniorLayout&>(l);
  }
  if (l.type() == LayoutNode::Type::JuniorLayout) {
    return static_cast<JuniorLayout&>(l);
  }
  PoincareJ::Tree* tree = PoincareJ::Layout::FromPoincareLayout(l);
  JuniorLayout j = Builder(tree);
  tree->removeTree();
  return j;
}

}  // namespace Poincare
