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
  PoincareJ::Render::Draw(tree(), ctx, p, style.font, style.glyphColor,
                          style.backgroundColor);
}

size_t JuniorLayoutNode::serialize(char* buffer, size_t bufferSize,
                                   Preferences::PrintFloatMode floatDisplayMode,
                                   int numberOfSignificantDigits) const {
  OLayout l = PoincareJ::Layout::ToPoincareLayout(tree());
  return l.node()->serialize(buffer, bufferSize, floatDisplayMode,
                             numberOfSignificantDigits);
}

OLayout JuniorLayoutNode::makeEditable() {
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

JuniorLayout JuniorLayout::Juniorize(OLayout l) {
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

void JuniorLayout::draw(KDContext* ctx, KDPoint p, KDGlyph::Style style,
                        const PoincareJ::LayoutSelection& selection,
                        KDColor selectionColor) {
  node()->draw(ctx, p, style, selection, selectionColor);
}

void JuniorLayout::draw(KDContext* ctx, KDPoint p, KDGlyph::Style style) {
  draw(ctx, p, style, PoincareJ::LayoutSelection());
}

// Rendering

void JuniorLayoutNode::draw(KDContext* ctx, KDPoint p, KDGlyph::Style style,
                            const PoincareJ::LayoutSelection& selection,
                            KDColor selectionColor) {
  PoincareJ::Render::Draw(tree(), ctx, p, style.font, style.glyphColor,
                          style.backgroundColor, selection);
}

}  // namespace Poincare
